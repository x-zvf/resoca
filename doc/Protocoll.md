# ReSoCa protocoll documentation

===

### Overview

The client and server communicate by sending lenght-prefixed Protobuf messages.

More specifically, a 16-bit unsigned short, with LSB-endianness is sent before
the protobuf message, which may be at most 1024 bytes long.

The Protobuf message format is defined in `src/shared/protobuf/ResocaMessage.proto`.

### Messages

The Message being sent is of type `ResocaMessage`, also referred to as the "root message".

There are two message types, Request and Response. Which one the message contains,
is denoted by the `isResponse` boolean.

Depending on the state of this field, there is either a `request` or `response`
submessage in the root message.

#### Request

The message type is decided by the value of the `requestType` enum.

#### RequestTypes

Valid types and their purpose are:

| Enum ID | Name | Purpose | Required Fields | Optional Fields |
| ------- | ---- | ------- | ------ |
| 0 | `PING` | Request to send a `PONG` message with the same `responseID` as the `requestID` of this message | `requestID` | - |
