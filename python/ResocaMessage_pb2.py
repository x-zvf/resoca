# -*- coding: utf-8 -*-
# Generated by the protocol buffer compiler.  DO NOT EDIT!
# source: src/shared/protobuf/ResocaMessage.proto

from google.protobuf import descriptor as _descriptor
from google.protobuf import message as _message
from google.protobuf import reflection as _reflection
from google.protobuf import symbol_database as _symbol_database
# @@protoc_insertion_point(imports)

_sym_db = _symbol_database.Default()




DESCRIPTOR = _descriptor.FileDescriptor(
  name='src/shared/protobuf/ResocaMessage.proto',
  package='',
  syntax='proto3',
  serialized_options=None,
  serialized_pb=b'\n\'src/shared/protobuf/ResocaMessage.proto\"\xc4\x07\n\rResocaMessage\x12\x12\n\nisResponse\x18\x01 \x01(\x08\x12\'\n\x07request\x18\x02 \x01(\x0b\x32\x16.ResocaMessage.Request\x12)\n\x08response\x18\x03 \x01(\x0b\x32\x17.ResocaMessage.Response\x1a\x9c\x01\n\x08\x43\x61nFrame\x12\r\n\x05\x63\x61nID\x18\x01 \x01(\r\x12\x0f\n\x07isCanFD\x18\x02 \x01(\x08\x12\x12\n\nisEFFFRAME\x18\x03 \x01(\x08\x12\x12\n\nisRTRFRAME\x18\x04 \x01(\x08\x12\x12\n\nisERRFRAME\x18\x05 \x01(\x08\x12\x12\n\nisCanFDESI\x18\x06 \x01(\x08\x12\x12\n\nisCanFDBRS\x18\x07 \x01(\x08\x12\x0c\n\x04\x64\x61ta\x18\n \x01(\x0c\x1a\xe1\x01\n\x07Request\x12\x37\n\x0brequestType\x18\x01 \x01(\x0e\x32\".ResocaMessage.Request.RequestType\x12\x11\n\trequestID\x18\x02 \x01(\r\x12\x0e\n\x06ifName\x18\x03 \x01(\t\x12)\n\x08\x63\x61nFrame\x18\x04 \x01(\x0b\x32\x17.ResocaMessage.CanFrame\"O\n\x0bRequestType\x12\x08\n\x04PING\x10\x00\x12\x10\n\x0cNOTIFY_START\x10\x01\x12\x0e\n\nNOTIFY_END\x10\x02\x12\n\n\x06\x43\x41N_TX\x10\x03\x12\x08\n\x04INFO\x10\x04\x1a\xc7\x03\n\x08Response\x12:\n\x0cresponseType\x18\x01 \x01(\x0e\x32$.ResocaMessage.Response.ResponseType\x12\x12\n\nresponseID\x18\x02 \x01(\r\x12\x13\n\x0b\x64\x65scription\x18\x03 \x01(\t\x12\x0e\n\x06ifName\x18\x04 \x01(\t\x12)\n\x08\x63\x61nFrame\x18\x05 \x01(\x0b\x32\x17.ResocaMessage.CanFrame\x12\x36\n\nresocaInfo\x18\x06 \x01(\x0b\x32\".ResocaMessage.Response.ResocaInfo\x1aH\n\nResocaInfo\x12\x0f\n\x07version\x18\x01 \x01(\t\x12\x12\n\ninterfaces\x18\x02 \x03(\t\x12\x15\n\rsessionPrefix\x18\x03 \x01(\r\"\x98\x01\n\x0cResponseType\x12\x07\n\x03\x41\x43K\x10\x00\x12\x0b\n\x07SUCCESS\x10\x01\x12\x07\n\x03\x45RR\x10\x02\x12\x08\n\x04PONG\x10\x03\x12\n\n\x06\x43\x41N_RX\x10\x04\x12\n\n\x06\x43\x41N_TX\x10\x05\x12\x0e\n\nCAN_TX_ERR\x10\x06\x12\x14\n\x10\x43\x41N_IF_CONNECTED\x10\x07\x12\x17\n\x13\x43\x41N_IF_DISCONNECTED\x10\x08\x12\x08\n\x04INFO\x10\tb\x06proto3'
)



_RESOCAMESSAGE_REQUEST_REQUESTTYPE = _descriptor.EnumDescriptor(
  name='RequestType',
  full_name='ResocaMessage.Request.RequestType',
  filename=None,
  file=DESCRIPTOR,
  values=[
    _descriptor.EnumValueDescriptor(
      name='PING', index=0, number=0,
      serialized_options=None,
      type=None),
    _descriptor.EnumValueDescriptor(
      name='NOTIFY_START', index=1, number=1,
      serialized_options=None,
      type=None),
    _descriptor.EnumValueDescriptor(
      name='NOTIFY_END', index=2, number=2,
      serialized_options=None,
      type=None),
    _descriptor.EnumValueDescriptor(
      name='CAN_TX', index=3, number=3,
      serialized_options=None,
      type=None),
    _descriptor.EnumValueDescriptor(
      name='INFO', index=4, number=4,
      serialized_options=None,
      type=None),
  ],
  containing_type=None,
  serialized_options=None,
  serialized_start=471,
  serialized_end=550,
)
_sym_db.RegisterEnumDescriptor(_RESOCAMESSAGE_REQUEST_REQUESTTYPE)

_RESOCAMESSAGE_RESPONSE_RESPONSETYPE = _descriptor.EnumDescriptor(
  name='ResponseType',
  full_name='ResocaMessage.Response.ResponseType',
  filename=None,
  file=DESCRIPTOR,
  values=[
    _descriptor.EnumValueDescriptor(
      name='ACK', index=0, number=0,
      serialized_options=None,
      type=None),
    _descriptor.EnumValueDescriptor(
      name='SUCCESS', index=1, number=1,
      serialized_options=None,
      type=None),
    _descriptor.EnumValueDescriptor(
      name='ERR', index=2, number=2,
      serialized_options=None,
      type=None),
    _descriptor.EnumValueDescriptor(
      name='PONG', index=3, number=3,
      serialized_options=None,
      type=None),
    _descriptor.EnumValueDescriptor(
      name='CAN_RX', index=4, number=4,
      serialized_options=None,
      type=None),
    _descriptor.EnumValueDescriptor(
      name='CAN_TX', index=5, number=5,
      serialized_options=None,
      type=None),
    _descriptor.EnumValueDescriptor(
      name='CAN_TX_ERR', index=6, number=6,
      serialized_options=None,
      type=None),
    _descriptor.EnumValueDescriptor(
      name='CAN_IF_CONNECTED', index=7, number=7,
      serialized_options=None,
      type=None),
    _descriptor.EnumValueDescriptor(
      name='CAN_IF_DISCONNECTED', index=8, number=8,
      serialized_options=None,
      type=None),
    _descriptor.EnumValueDescriptor(
      name='INFO', index=9, number=9,
      serialized_options=None,
      type=None),
  ],
  containing_type=None,
  serialized_options=None,
  serialized_start=856,
  serialized_end=1008,
)
_sym_db.RegisterEnumDescriptor(_RESOCAMESSAGE_RESPONSE_RESPONSETYPE)


_RESOCAMESSAGE_CANFRAME = _descriptor.Descriptor(
  name='CanFrame',
  full_name='ResocaMessage.CanFrame',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='canID', full_name='ResocaMessage.CanFrame.canID', index=0,
      number=1, type=13, cpp_type=3, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='isCanFD', full_name='ResocaMessage.CanFrame.isCanFD', index=1,
      number=2, type=8, cpp_type=7, label=1,
      has_default_value=False, default_value=False,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='isEFFFRAME', full_name='ResocaMessage.CanFrame.isEFFFRAME', index=2,
      number=3, type=8, cpp_type=7, label=1,
      has_default_value=False, default_value=False,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='isRTRFRAME', full_name='ResocaMessage.CanFrame.isRTRFRAME', index=3,
      number=4, type=8, cpp_type=7, label=1,
      has_default_value=False, default_value=False,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='isERRFRAME', full_name='ResocaMessage.CanFrame.isERRFRAME', index=4,
      number=5, type=8, cpp_type=7, label=1,
      has_default_value=False, default_value=False,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='isCanFDESI', full_name='ResocaMessage.CanFrame.isCanFDESI', index=5,
      number=6, type=8, cpp_type=7, label=1,
      has_default_value=False, default_value=False,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='isCanFDBRS', full_name='ResocaMessage.CanFrame.isCanFDBRS', index=6,
      number=7, type=8, cpp_type=7, label=1,
      has_default_value=False, default_value=False,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='data', full_name='ResocaMessage.CanFrame.data', index=7,
      number=10, type=12, cpp_type=9, label=1,
      has_default_value=False, default_value=b"",
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  serialized_options=None,
  is_extendable=False,
  syntax='proto3',
  extension_ranges=[],
  oneofs=[
  ],
  serialized_start=166,
  serialized_end=322,
)

_RESOCAMESSAGE_REQUEST = _descriptor.Descriptor(
  name='Request',
  full_name='ResocaMessage.Request',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='requestType', full_name='ResocaMessage.Request.requestType', index=0,
      number=1, type=14, cpp_type=8, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='requestID', full_name='ResocaMessage.Request.requestID', index=1,
      number=2, type=13, cpp_type=3, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='ifName', full_name='ResocaMessage.Request.ifName', index=2,
      number=3, type=9, cpp_type=9, label=1,
      has_default_value=False, default_value=b"".decode('utf-8'),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='canFrame', full_name='ResocaMessage.Request.canFrame', index=3,
      number=4, type=11, cpp_type=10, label=1,
      has_default_value=False, default_value=None,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
    _RESOCAMESSAGE_REQUEST_REQUESTTYPE,
  ],
  serialized_options=None,
  is_extendable=False,
  syntax='proto3',
  extension_ranges=[],
  oneofs=[
  ],
  serialized_start=325,
  serialized_end=550,
)

_RESOCAMESSAGE_RESPONSE_RESOCAINFO = _descriptor.Descriptor(
  name='ResocaInfo',
  full_name='ResocaMessage.Response.ResocaInfo',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='version', full_name='ResocaMessage.Response.ResocaInfo.version', index=0,
      number=1, type=9, cpp_type=9, label=1,
      has_default_value=False, default_value=b"".decode('utf-8'),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='interfaces', full_name='ResocaMessage.Response.ResocaInfo.interfaces', index=1,
      number=2, type=9, cpp_type=9, label=3,
      has_default_value=False, default_value=[],
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='sessionPrefix', full_name='ResocaMessage.Response.ResocaInfo.sessionPrefix', index=2,
      number=3, type=13, cpp_type=3, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  serialized_options=None,
  is_extendable=False,
  syntax='proto3',
  extension_ranges=[],
  oneofs=[
  ],
  serialized_start=781,
  serialized_end=853,
)

_RESOCAMESSAGE_RESPONSE = _descriptor.Descriptor(
  name='Response',
  full_name='ResocaMessage.Response',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='responseType', full_name='ResocaMessage.Response.responseType', index=0,
      number=1, type=14, cpp_type=8, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='responseID', full_name='ResocaMessage.Response.responseID', index=1,
      number=2, type=13, cpp_type=3, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='description', full_name='ResocaMessage.Response.description', index=2,
      number=3, type=9, cpp_type=9, label=1,
      has_default_value=False, default_value=b"".decode('utf-8'),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='ifName', full_name='ResocaMessage.Response.ifName', index=3,
      number=4, type=9, cpp_type=9, label=1,
      has_default_value=False, default_value=b"".decode('utf-8'),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='canFrame', full_name='ResocaMessage.Response.canFrame', index=4,
      number=5, type=11, cpp_type=10, label=1,
      has_default_value=False, default_value=None,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='resocaInfo', full_name='ResocaMessage.Response.resocaInfo', index=5,
      number=6, type=11, cpp_type=10, label=1,
      has_default_value=False, default_value=None,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
  ],
  extensions=[
  ],
  nested_types=[_RESOCAMESSAGE_RESPONSE_RESOCAINFO, ],
  enum_types=[
    _RESOCAMESSAGE_RESPONSE_RESPONSETYPE,
  ],
  serialized_options=None,
  is_extendable=False,
  syntax='proto3',
  extension_ranges=[],
  oneofs=[
  ],
  serialized_start=553,
  serialized_end=1008,
)

_RESOCAMESSAGE = _descriptor.Descriptor(
  name='ResocaMessage',
  full_name='ResocaMessage',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='isResponse', full_name='ResocaMessage.isResponse', index=0,
      number=1, type=8, cpp_type=7, label=1,
      has_default_value=False, default_value=False,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='request', full_name='ResocaMessage.request', index=1,
      number=2, type=11, cpp_type=10, label=1,
      has_default_value=False, default_value=None,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='response', full_name='ResocaMessage.response', index=2,
      number=3, type=11, cpp_type=10, label=1,
      has_default_value=False, default_value=None,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
  ],
  extensions=[
  ],
  nested_types=[_RESOCAMESSAGE_CANFRAME, _RESOCAMESSAGE_REQUEST, _RESOCAMESSAGE_RESPONSE, ],
  enum_types=[
  ],
  serialized_options=None,
  is_extendable=False,
  syntax='proto3',
  extension_ranges=[],
  oneofs=[
  ],
  serialized_start=44,
  serialized_end=1008,
)

_RESOCAMESSAGE_CANFRAME.containing_type = _RESOCAMESSAGE
_RESOCAMESSAGE_REQUEST.fields_by_name['requestType'].enum_type = _RESOCAMESSAGE_REQUEST_REQUESTTYPE
_RESOCAMESSAGE_REQUEST.fields_by_name['canFrame'].message_type = _RESOCAMESSAGE_CANFRAME
_RESOCAMESSAGE_REQUEST.containing_type = _RESOCAMESSAGE
_RESOCAMESSAGE_REQUEST_REQUESTTYPE.containing_type = _RESOCAMESSAGE_REQUEST
_RESOCAMESSAGE_RESPONSE_RESOCAINFO.containing_type = _RESOCAMESSAGE_RESPONSE
_RESOCAMESSAGE_RESPONSE.fields_by_name['responseType'].enum_type = _RESOCAMESSAGE_RESPONSE_RESPONSETYPE
_RESOCAMESSAGE_RESPONSE.fields_by_name['canFrame'].message_type = _RESOCAMESSAGE_CANFRAME
_RESOCAMESSAGE_RESPONSE.fields_by_name['resocaInfo'].message_type = _RESOCAMESSAGE_RESPONSE_RESOCAINFO
_RESOCAMESSAGE_RESPONSE.containing_type = _RESOCAMESSAGE
_RESOCAMESSAGE_RESPONSE_RESPONSETYPE.containing_type = _RESOCAMESSAGE_RESPONSE
_RESOCAMESSAGE.fields_by_name['request'].message_type = _RESOCAMESSAGE_REQUEST
_RESOCAMESSAGE.fields_by_name['response'].message_type = _RESOCAMESSAGE_RESPONSE
DESCRIPTOR.message_types_by_name['ResocaMessage'] = _RESOCAMESSAGE
_sym_db.RegisterFileDescriptor(DESCRIPTOR)

ResocaMessage = _reflection.GeneratedProtocolMessageType('ResocaMessage', (_message.Message,), {

  'CanFrame' : _reflection.GeneratedProtocolMessageType('CanFrame', (_message.Message,), {
    'DESCRIPTOR' : _RESOCAMESSAGE_CANFRAME,
    '__module__' : 'src.shared.protobuf.ResocaMessage_pb2'
    # @@protoc_insertion_point(class_scope:ResocaMessage.CanFrame)
    })
  ,

  'Request' : _reflection.GeneratedProtocolMessageType('Request', (_message.Message,), {
    'DESCRIPTOR' : _RESOCAMESSAGE_REQUEST,
    '__module__' : 'src.shared.protobuf.ResocaMessage_pb2'
    # @@protoc_insertion_point(class_scope:ResocaMessage.Request)
    })
  ,

  'Response' : _reflection.GeneratedProtocolMessageType('Response', (_message.Message,), {

    'ResocaInfo' : _reflection.GeneratedProtocolMessageType('ResocaInfo', (_message.Message,), {
      'DESCRIPTOR' : _RESOCAMESSAGE_RESPONSE_RESOCAINFO,
      '__module__' : 'src.shared.protobuf.ResocaMessage_pb2'
      # @@protoc_insertion_point(class_scope:ResocaMessage.Response.ResocaInfo)
      })
    ,
    'DESCRIPTOR' : _RESOCAMESSAGE_RESPONSE,
    '__module__' : 'src.shared.protobuf.ResocaMessage_pb2'
    # @@protoc_insertion_point(class_scope:ResocaMessage.Response)
    })
  ,
  'DESCRIPTOR' : _RESOCAMESSAGE,
  '__module__' : 'src.shared.protobuf.ResocaMessage_pb2'
  # @@protoc_insertion_point(class_scope:ResocaMessage)
  })
_sym_db.RegisterMessage(ResocaMessage)
_sym_db.RegisterMessage(ResocaMessage.CanFrame)
_sym_db.RegisterMessage(ResocaMessage.Request)
_sym_db.RegisterMessage(ResocaMessage.Response)
_sym_db.RegisterMessage(ResocaMessage.Response.ResocaInfo)


# @@protoc_insertion_point(module_scope)
