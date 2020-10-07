using System;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Timers;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

using ResocaClientLib;

namespace ResocaClientGUI
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        ResocaClient resocaClient;
        private Timer resocaPollTimer;

        public class DisplayedCanFrame
        {
            public string RxTx { get; set; }
            public string IfName { get; set; }
            public string Flags { get; set; }
            public string CanId { get; set; }
            public string Data { get; set; }

            public DisplayedCanFrame(string rxTx,string ifName, CanFrame f)
            {
                this.RxTx = rxTx;
                this.IfName = ifName;
                this.CanId = f.canID.ToString("X");
                this.Flags = "";
                if (f.isCanFD) Flags += " FD";
                if (f.isEFFFRAME) Flags += " EFF";
                if (f.isRTRFRAME) Flags += " RTR";
                if (f.isERRFRAME) Flags += " ERR";
                if (f.isCanFDBRS) Flags += " BRS";
                if (f.isCanFDESI) Flags += " ESI";
                if(Flags.Length > 0)
                    Flags = Flags.Substring(1, Flags.Length - 1);
                this.Data = BitConverter.ToString(f.data).Replace('-', ' ');
            }
        }

        public MainWindow()
        {
            InitializeComponent();
            resocaClient = new ResocaClient();
            resocaClient.OnInfoReceivedHandlers.AddFirst(delegate ()
            {
                Dispatcher.Invoke(() =>
                {


                    while (InterfaceCb.Items.Count > 0)
                    {
                        InterfaceCb.Items.RemoveAt(0);
                    }

                    while (NotifyILv.Items.Count > 0)
                    {
                        NotifyILv.Items.RemoveAt(0);
                    }

                    foreach (string aif in resocaClient.AvailableInterfaces)
                    {
                        InterfaceCb.Items.Add(aif);
                        NotifyILv.Items.Add(aif);
                    }

                    ServerVersionLbl.Content = resocaClient.ServerVersion;
                    SessionPrefixLbl.Content = resocaClient.SessionPrefix.ToString("X");
                });
            });

            resocaClient.OnCanFrameReceivedHandlers.AddFirst(delegate (string ifName, CanFrame frame)
            {
                Dispatcher.Invoke(() =>
                {
                    RxMessagesLv.Items.Insert(0, new DisplayedCanFrame("Rx", ifName, frame));
                    if (RxMessagesLv.Items.Count > 50)
                    {
                        RxMessagesLv.Items.RemoveAt(RxMessagesLv.Items.Count - 1);
                    }
                });
            });

            resocaClient.OnCanFrameSentHandlers.AddFirst(delegate (bool success, string err, string ifName,
                CanFrame frame)
            {
                Dispatcher.Invoke(() =>
                {
                    if (success)
                    {
                        LastMessageStatusLb.Content = "success";
                        RxMessagesLv.Items.Insert(0, new DisplayedCanFrame("Tx", ifName, frame));
                        if (RxMessagesLv.Items.Count > 50)
                        {
                            RxMessagesLv.Items.RemoveAt(RxMessagesLv.Items.Count - 1);
                        }
                    }
                    else
                    {
                        MessageBox.Show($"Failed to send message on {ifName}: \n{err}");
                        LastMessageStatusLb.Content = "failed";
                    }


                });
            });

            resocaClient.OnPingReceivedHandlers.AddFirst(delegate(double time)
            {
                var text = $"{time*1000:000.}ms";
                Dispatcher.Invoke(() =>
                {
                    PingLbl.Content = text;
                });
            });


            resocaPollTimer = new Timer(1);
            resocaPollTimer.Elapsed += OnTimedEvent;
            resocaPollTimer.AutoReset = true;
        }


        private void OnTimedEvent(Object source, ElapsedEventArgs e)
        {
            resocaClient.ReadPoll();
            if (!resocaClient.IsConnected())
            {
                try
                {
                    Dispatcher.Invoke(() => { ConnectBtn.Content = "connect"; });
                }
                catch (Exception ex)
                {
                }
            }
        }

        private void ConnectBtn_Click(object sender, RoutedEventArgs e)
        {
            resocaClient.Host = HostTB.Text;
            resocaPollTimer.Stop();
            try
            {
                resocaClient.Port = int.Parse(PortTB.Text);
            }
            catch (Exception exception)
            {
                MessageBox.Show("Invalid Port number", "Invalid Port Number", MessageBoxButton.OK,
                    MessageBoxImage.Error);
                return;
            }

            ConnectBtn.Content = "connecting...";

            bool res = resocaClient.Connect();
            if (res && resocaClient.IsConnected())
            {
                ConnectBtn.Content = "connected";

                resocaPollTimer.Start();
                resocaClient.GetInfo();
            }
            else
            {
                MessageBox.Show($"Failed to connect to server at {resocaClient.Host}:{resocaClient.Port}",
                    "Connection Error", MessageBoxButton.OK, MessageBoxImage.Error);
                ConnectBtn.Content = "connect";
            }
        }

        private void canIdTb_TextChanged(object sender, TextChangedEventArgs e)
        {
            if (canIdTb == null || canFdCb == null) return;
            string t = canIdTb.Text;
            string nt = "";
            foreach (var t1 in t)
            {
                char[] validChars = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
                if (validChars.Contains(t1)) nt += t1;
            }

            canIdTb.Background =
                int.TryParse(nt, NumberStyles.HexNumber, null, out int val) && val > 0 &&
                ((effFrameCb.IsChecked == true && val <= 0x1fffffff) | (!effFrameCb.IsChecked == true && val <= 0x7ff))
                    ? new SolidColorBrush(Color.FromRgb(0, 0xFF, 0))
                    : new SolidColorBrush(Color.FromRgb(0xFF, 0, 0));
            string ft = "";
            for (int i = 0; i < nt.Length; i++)
            {
                ft += nt[i];
                if (i > 0 && i % 2 == 1 && i < nt.Length - 1) ft += ' ';
            }

            canIdTb.Text = ft;
            if (sender != null)
                canIdTb.SelectionStart = ft.Length;
        }

        private void canFdCb_Clicked(object sender, RoutedEventArgs e)
        {
            canIdTb_TextChanged(null, null);
            DataTb_TextChanged(null, null);
        }

        private void NotifyStartBtn_OnClick(object sender, RoutedEventArgs e)
        {
            resocaClient.NotifyStart(((Button)sender).DataContext.ToString());
        }

        private void NotifyEndBtn_OnClick(object sender, RoutedEventArgs e)
        {
            resocaClient.NotifyEnd(((Button)sender).DataContext.ToString());
        }

        private void DataTb_TextChanged(object sender, TextChangedEventArgs e)
        {
            if (DataTb == null || UTF8Rb == null || canFdCb == null) return;
            int nBytesAvail = canFdCb.IsChecked == true ? 64 : 8;
            int nBytes = 0;
            if (UTF8Rb.IsChecked == true)
            {
                byte[] ncodedBytes = Encoding.UTF8.GetBytes(DataTb.Text);
                nBytes = ncodedBytes.Length;
            }
            else
            {
                string t = DataTb.Text;
                string nt = "";
                foreach (var t1 in t)
                {
                    char[] validChars =
                        {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
                    if (validChars.Contains(t1)) nt += t1;
                }

                nBytes = (int)Math.Ceiling(nt.Length / 2.0f);
                string ft = "";
                for (int i = 0; i < nt.Length; i++)
                {
                    ft += nt[i];
                    if (i > 0 && i % 2 == 1 && i < nt.Length - 1) ft += ' ';
                }

                DataTb.Text = ft;
                if (sender != null)
                    DataTb.SelectionStart = ft.Length;
            }

            BytesUsedLb.Content = $"{nBytes}/{nBytesAvail} bytes";
            DataTb.Background = nBytes <= nBytesAvail
                ? new SolidColorBrush(Color.FromRgb(0, 0xFF, 0))
                : new SolidColorBrush(Color.FromRgb(0xFF, 0, 0));
        }

        private void HexRb_Clicked(object sender, RoutedEventArgs e)
        {
            if (UTF8Rb != null && UTF8Rb.IsChecked == true) 
                DataTb.CharacterCasing = CharacterCasing.Normal;
            else 
                DataTb.CharacterCasing = CharacterCasing.Upper;
            DataTb_TextChanged(null, null);
        }

        private void SendFrameBtn_Click(object sender, RoutedEventArgs e)
        {
            string ifn = InterfaceCb.SelectedItem.ToString();
            string t = canIdTb.Text;
            string nt = "";
            foreach (var t1 in t)
            {
                char[] validChars = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
                if (validChars.Contains(t1)) nt += t1;
            }

            if (!uint.TryParse(nt, NumberStyles.HexNumber, null, out uint cid))
            {
                MessageBox.Show("Invalid CAN ID", "Frame error", MessageBoxButton.OK, MessageBoxImage.Error);
                return;
            }

            CanFrame cf = new CanFrame();
            cf.canID = cid;
            cf.isCanFD = canFdCb.IsChecked == true;
            cf.isEFFFRAME = effFrameCb.IsChecked == true;
            if ((cf.isEFFFRAME && cid > 0x1fffffff) || (!cf.isEFFFRAME && cid > 0x7ff))
            {
                MessageBox.Show(" CAN ID out of range", "Frame error", MessageBoxButton.OK, MessageBoxImage.Error);
                return;
            }

            cf.isERRFRAME = errFrameCb.IsChecked == true;
            cf.isRTRFRAME = rtrFrameCb.IsChecked == true;
            cf.isCanFDBRS = canFdBRS.IsChecked == true;
            cf.isCanFDESI = canFdESI.IsChecked == true;
            if (!cf.isCanFD && (cf.isCanFDBRS || cf.isCanFDESI))
            {
                MessageBox.Show("BRS and ESI flags may only be used in a CANFD frame", "Frame error",
                    MessageBoxButton.OK, MessageBoxImage.Error);
                return;
            }

            byte[] codedBytes;
            if (UTF8Rb.IsChecked == true)
            {
                codedBytes = Encoding.UTF8.GetBytes(DataTb.Text);
            }
            else
            {
                string dt = DataTb.Text;
                string dnt = "";
                foreach (var t1 in dt)
                {
                    char[] validChars =
                        {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
                    if (validChars.Contains(t1)) dnt += t1;
                }

                if (dnt.Length % 2 == 1) dnt += "0";
                codedBytes = StringToByteArrayFastest(dnt);
            }

            cf.data = codedBytes;
            if ((cf.isCanFD && cf.data.Length > 64) || (!cf.isCanFD && cf.data.Length > 8))
            {
                MessageBox.Show("Data is too long", "Frame error", MessageBoxButton.OK, MessageBoxImage.Error);
                return;
            }

            SendFrameBtn.Content = "sending";
            LastMessageStatusLb.Content = "";

            resocaClient.SendCanFrame(ifn, cf);
            SendFrameBtn.Content = "send";
            //LastMessageStatusLb.Content = "waiting";

        }

        //from: https://stackoverflow.com/questions/321370/how-can-i-convert-a-hex-string-to-a-byte-array
        public static byte[] StringToByteArrayFastest(string hex)
        {
            if (hex.Length % 2 == 1)
                throw new Exception("The binary key cannot have an odd number of digits");

            byte[] arr = new byte[hex.Length >> 1];

            for (int i = 0; i < hex.Length >> 1; ++i)
            {
                arr[i] = (byte)((GetHexVal(hex[i << 1]) << 4) + (GetHexVal(hex[(i << 1) + 1])));
            }

            return arr;
        }

        public static int GetHexVal(char hex)
        {
            int val = (int)hex;
            //For uppercase A-F letters:
            //return val - (val < 58 ? 48 : 55);
            //For lowercase a-f letters:
            //return val - (val < 58 ? 48 : 87);
            //Or the two combined, but a bit slower:
            return val - (val < 58 ? 48 : (val < 97 ? 55 : 87));
        }

        private void PingBtn_Click(object sender, RoutedEventArgs e)
        {
            if (resocaClient.IsConnected())
            {
                resocaClient.Ping();
            }
        }
    }
}
