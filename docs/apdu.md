# RENEC Ledger Embedded App communication protocol

## RENEC app communication

The communication protocol used by [BOLOS](https://developers.ledger.com/docs/embedded-app/bolos-introduction/) to exchange [APDU](https://en.wikipedia.org/wiki/Smart_card_application_protocol_data_unit) (a decent overview of APDU protocol can be found [here](http://cardwerk.com/smart-card-standard-iso7816-4-section-5-basic-organizations/#chap5_4)). RENEC app communicates with APDU protocol, each "logical" call consists of a series of APDU exchanges where APDU is in the following form

### Command

| Field name | Length (bytes) | Description                                                           |
| ---------- | -------------- | --------------------------------------------------------------------- |
| CLA        | 1              | Instruction class - indicates the type of command                     |
| INS        | 1              | Instruction code - indicates the specific command                     |
| P1         | 1              | Instruction parameter 1 for the command                               |
| P2         | 1              | Instruction parameter 2 for the command                               |
| Lc         | 1              | The number of bytes of command data to follow (a value from 0 to 255) |
| CData      | var            | Command data with `Lc` bytes                                          |

## Response

| Field name | Length (bytes) | Description                                                                  |
| ---------- | -------------- | ---------------------------------------------------------------------------- |
| RData      | var            | Response data (can be empty)                                                 |
| SW         | 2              | Status word containing command processing status (e.g. `0x9000` for success) |

Known error codes are:

- 0x9000 = OK
- see [src/apdu/errors.h](../src/apdu/errors.h) for the full listing of other errors.

## Instructions

Instructions are split into several groups depending on their purpose. See [src/handlers.c](../src/handlers.c) for full listing

### `INS=0x0*` group

Instructions related to general app status

- `0x00`: [Get app version](ins_get_app_version.md)
- `0x01`: [Get device serial number](ins_get_serial_number.md)

## Status Words

The following standard Status Words are returned for all APDUs - some specific Status Words can be used for specific commands and are mentioned in the command description.

| _SW_   |                   _Description_                   |
| ------ | :-----------------------------------------------: |
| 0x6700 |                 Incorrect length                  |
| 0x6982 | Security status not satisfied (Canceled by user)  |
| 0x6A80 |                   Invalid data                    |
| 0x6A81 |         Invalid off-chain message header          |
| 0x6A82 |         Invalid off-chain message format          |
| 0x6B00 |           Incorrect parameter P1 or P2            |
| 0x6Fxx | Technical problem (Internal error, please report) |
| 0x9000 |           Normal ending of the command            |

## Protocol upgrade considerations:

In order to ensure safe forward/backward compatibility, sender _must_ set any _unused_ field to zero. When upgrading protocol, any unused field that is no longer unused _must_ define only values != 0. This will ensure that clients using old protocol will receive errors instead of an unexpected behavior.
