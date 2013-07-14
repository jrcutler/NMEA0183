NMEA0183 State Machine
======================

Character Groups
----------------

|Name    |Value          |
|--------|---------------|
|RESERVED|`\x0a\x0d!$*,^`|
|FUTURE  |`\x5c\x7e-\xff`|
|CR      |`\x0d`         |
|LF      |`\x0a`         |

Rules
-----

(All rules are in priority order.)

|Current state    |Input   |Next state       |
|-----------------|--------|-----------------|
|*                |FUTURE  |RESET            |
|*                |`$`     |ADDRESS          |
|ADDRESS          |`A-Z0-9`|ADDRESS          |
|ADDRESS          |`,`     |FIELD_DATA       |
|FIELD_DATA       |`,`     |FIELD_DATA       |
|FIELD_DATA       |`^`     |FIELD_DATA_HEX_HI|
|FIELD_DATA_HEX_HI|`0-9A-F`|FIELD_DATA_HEX_LO|
|FIELD_DATA_HEX_LO|`0-9A-F`|FIELD_DATA       |
|FIELD_DATA       |`*`     |CHECKSUM_HI      |
|CHECKSUM_HI      |`0-9A-F`|CHECKSUM_LO      |
|CHECKSUM_LO      |`0-9A-F`|EOS_CR           |
|EOS_CR           |CR      |EOS_LF           |
|EOS_LF           |LF      |ACCEPT           |
|*                |RESERVED|RESET            |
|FIELD_DATA       |*       |FIELD_DATA       |
|*                |*       |RESET            |
