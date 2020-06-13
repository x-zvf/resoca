using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Net;
using System.Net.Sockets;
using System.Diagnostics;
using System.Threading;

namespace ResocaClientLib
{
    public class ResocaClient
    {
        public string Host = "127.0.0.1";
        public int Port = 23636;

        public string ServerVersion { get; private set; }
        public string[] AvailableInterfaces { get; private set; }
        public uint SessionPrefix { get; private set; }

        private ushort SendCounter = 0;

        // Tuple is (messageType, hasACK)
        public Dictionary<uint, (ResocaMessage.Types.Request.Types.RequestType, bool)> MessagesPending { get; private set; }
        public bool IsConnected()
        {
            if (socket == null) return false;
            return socket.Connected;
        }

        public double latestPingTime { get; private set; } = 0;
        private Stopwatch pingWatch = new Stopwatch();

        public delegate void OnConnectionChangedHandler(bool connected);
        LinkedList<OnConnectionChangedHandler> OnConnectionChangedHandlers;

        public delegate void OnInterfaceStateChangeHandler(string ifName, bool connected);
        LinkedList<OnInterfaceStateChangeHandler> OnInterfaceStateChangeHandlers;


        public delegate void OnCanFrameReceivedHandler(string ifName, CanFrame frame);
        LinkedList<OnCanFrameReceivedHandler> OnCanFrameReceivedHandlers;

        public delegate void OnCanFrameSentHandler(bool success, string err, string ifName, CanFrame frame);
        LinkedList<OnCanFrameSentHandler> OnCanFrameSentHandlers;

        public delegate void OnPingReceivedHandler(double time);
        LinkedList<OnPingReceivedHandler> OnPingReceivedHandlers;

        public delegate void OnInfoReceivedHandler();
        LinkedList<OnInfoReceivedHandler> OnInfoReceivedHandlers;

        public delegate void OnGenericMessageResolveHandler(uint rid, ResocaMessage.Types.Request.Types.RequestType requestType, bool success, string description);
        LinkedList<OnGenericMessageResolveHandler> OnGenericMessageResolveHandlers;

        private IPEndPoint serverEndPoint;
        private Socket socket;

        public ResocaClient()
        {

        }

        public bool Connect()
        {
            if (IsConnected())
            {
                socket.Shutdown(SocketShutdown.Both);
                socket.Close();
            }

            try
            {
                IPHostEntry ipHost = Dns.GetHostEntry(Host);
                IPAddress ipAddr = ipHost.AddressList[0];
                serverEndPoint = new IPEndPoint(ipAddr, Port);
                socket = new Socket(ipAddr.AddressFamily, SocketType.Stream, ProtocolType.Tcp);

                socket.Connect(serverEndPoint);
                SendCounter = 0;
                return true;
            } catch (Exception e)
            {
                return false;
            }
        }

        public void GetInfo()
        {

        }

        public void Ping()
        {
            ResocaMessage rsm = new ResocaMessage();
            rsm.IsResponse = false;
            rsm.Request.RequestType = ResocaMessage.Types.Request.Types.RequestType.Ping;
            pingWatch.Start();
            SendManaged(rsm);
        }

        public void NotifyStart(string ifName)
        {
            ResocaMessage rsm = new ResocaMessage();
            rsm.IsResponse = false;
            rsm.Request.RequestType = ResocaMessage.Types.Request.Types.RequestType.NotifyStart;
            rsm.Request.IfName = ifName;
            SendManaged(rsm);
        }

        public void NotifyStop(string ifName)
        {
            ResocaMessage rsm = new ResocaMessage();
            rsm.IsResponse = false;
            rsm.Request.RequestType = ResocaMessage.Types.Request.Types.RequestType.NotifyStart;
            rsm.Request.IfName = ifName;
            SendManaged(rsm);

        }

        public void SendCanFrame(string ifName, CanFrame frame)
        {

        }

        public void ReadPoll()
        {
            ///<summary>
            /// Call this periodically.
            /// </summary>
            while (IsConnected() && socket.Available >= 2)
            {

                byte[] lbuf = new byte[2];
                socket.Receive(lbuf);
                ushort len = (ushort)((lbuf[1] << 8) + lbuf[0]);

                byte[] rbuf = new byte[len];
                socket.Receive(rbuf);
                ResocaMessage rsm = ResocaMessage.Parser.ParseFrom(rbuf);
                ParseRSM(rsm);
            }
        }

        private void ParseRSM(ResocaMessage rsm)
        {
            if (rsm.IsResponse)
            {
                var resp = rsm.Response;
                if (resp.ResponseType == ResocaMessage.Types.Response.Types.ResponseType.Ack)
                {
                    (ResocaMessage.Types.Request.Types.RequestType, bool) x;
                    if (!MessagesPending.TryGetValue(resp.ResponseID, out x)) return;
                    var (t, b) = x;
                    MessagesPending[resp.ResponseID] = (t, true);
                    return;
                }
                
                switch (resp.ResponseType)
                {
                    case ResocaMessage.Types.Response.Types.ResponseType.Success:
                    case ResocaMessage.Types.Response.Types.ResponseType.Err:

                        (ResocaMessage.Types.Request.Types.RequestType, bool) x;
                        if (!MessagesPending.TryGetValue(resp.ResponseID, out x)) return;
                        var (t, b) = x;
                        foreach (var handler in OnGenericMessageResolveHandlers)
                        {
                            handler(resp.ResponseID, t, resp.ResponseType == ResocaMessage.Types.Response.Types.ResponseType.Success, resp.Description);
                        }
                        break;
                    case ResocaMessage.Types.Response.Types.ResponseType.Pong:
                        pingWatch.Stop();
                        TimeSpan ts = pingWatch.Elapsed;
                        latestPingTime = ts.TotalSeconds;
                        foreach (var handler in OnPingReceivedHandlers)
                        {
                            handler(latestPingTime);
                        }
                        break;
                    case ResocaMessage.Types.Response.Types.ResponseType.CanRx:
                        var frame = CanFrameFromPB(resp.CanFrame);
                        foreach (var handler in OnCanFrameReceivedHandlers)
                        {
                            handler(resp.IfName, frame);
                        }
                        break;
                    case ResocaMessage.Types.Response.Types.ResponseType.CanTx:
                    case ResocaMessage.Types.Response.Types.ResponseType.CanTxErr:
                        var cframe = CanFrameFromPB(resp.CanFrame);
                        foreach (var handler in OnCanFrameSentHandlers)
                        {
                            handler(resp.ResponseType == ResocaMessage.Types.Response.Types.ResponseType.CanTx, resp.Description, resp.IfName, cframe);
                        }
                        break;
                    case ResocaMessage.Types.Response.Types.ResponseType.CanIfConnected:
                    case ResocaMessage.Types.Response.Types.ResponseType.CanIfDisconnected:
                        foreach (var handler in OnInterfaceStateChangeHandlers)
                        {
                            handler(resp.IfName, resp.ResponseType == ResocaMessage.Types.Response.Types.ResponseType.CanIfConnected);
                        }
                        break;
                    case ResocaMessage.Types.Response.Types.ResponseType.Info:
                        ServerVersion = resp.ResocaInfo.Version;
                        AvailableInterfaces = resp.ResocaInfo.Interfaces.ToArray<string>();
                        SessionPrefix = resp.ResocaInfo.SessionPrefix;
                        foreach (var handler in OnInfoReceivedHandlers)
                        {
                            handler();
                        }
                        break;
                }
                MessagesPending.Remove(resp.ResponseID);
            }
        }

        private CanFrame CanFrameFromPB(ResocaMessage.Types.CanFrame cf)
        {
            CanFrame frame = new CanFrame();
            frame.canID = cf.CanID;
            frame.isEFFFRAME = cf.IsEFFFRAME;
            frame.isRTRFRAME = cf.IsRTRFRAME;
            frame.isERRFRAME = cf.IsERRFRAME;
            frame.isCanFDESI = cf.IsCanFDESI;
            frame.isCanFDBRS = cf.IsCanFDBRS;
            frame.data = cf.Data.ToArray<byte>();
            return frame;
        }

        private bool SendManaged(ResocaMessage rsm)
        {
            if (!IsConnected() || SessionPrefix == 0) return false;
            uint msgID = SessionPrefix + SendCounter;
            rsm.Request.RequestID = msgID;
            MessagesPending[msgID] = (rsm.Request.RequestType, false);
            SendCounter++;
            return true;
        }

        private bool SendMessage(ResocaMessage rsm)
        {
            ushort len = (ushort)rsm.CalculateSize();
            if (!IsConnected()) return false;

            byte[] buf = new byte[2 + len];
            buf[0] = (byte)(len & 0xFF);
            buf[1] = (byte)((len & 0XFF00) >> 8);

            //TODO: HACK: Find a better way. C# doesn't seem to let you do &buf[2] like in C++
            byte[] pbuf = new byte[len];
            rsm.WriteTo(new Google.Protobuf.CodedOutputStream(pbuf));
            for (int i = 0; i < len; i++)
                buf[2 + i] = pbuf[i];

            socket.Send(buf);
            return true;
        }
    }

    public struct CanFrame
    {
        public uint canID;
        public bool isCanFD;
        public bool isEFFFRAME;
        public bool isRTRFRAME;
        public bool isERRFRAME;
        public bool isCanFDESI;
        public bool isCanFDBRS;
        public byte[] data;
    }
}
