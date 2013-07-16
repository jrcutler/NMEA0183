#include "NMEA0183.h"


static char to_hex(uint8_t nibble)
{
    nibble &= 0x0f;
    return nibble > 9 ? nibble - 10 + 'A' : nibble + '0';
}


static bool is_hex(char c)
{
    return (c >= '0' && c <= '9') || (c >= 'A' && c <= 'F');
}


NMEA0183::NMEA0183()
{
    reset();
}


void NMEA0183::reset()
{
    state = NMEA0183_INVALID;
    update_checksum = false;
    index = 0;
    fields = 0;
    checksum = 0;
}


bool NMEA0183::update(char c)
{
    if (state == NMEA0183_ACCEPT)
    {
        // discard previously accepted sentence
        reset();
    }

    if (!(c & 0x80) && (c >= 0x20))
    {
        switch (c)
        {
            case '!':
            case '$':
                // new sentence
                index = 0;
                fields = 0;
                checksum = 0;
                state = NMEA0183_ADDRESS;
            break;
            case '*':
                if (state == NMEA0183_FIELD_DATA || state == NMEA0183_ADDRESS)
                {
                    update_checksum = false;
                    state = NMEA0183_CHECKSUM_HI;
                }
                else
                {
                    reset();
                }
            break;
            case ',':
                if (state == NMEA0183_FIELD_DATA || state == NMEA0183_ADDRESS)
                {
                    state = NMEA0183_FIELD_DATA;
                    fields++;
                }
                else
                {
                    reset();
                }
            break;
            case '\\':
            case '~':
            case 0x7f:
                // reserved for future use
                reset();
            break;
            default:
                switch (state)
                {
                    case NMEA0183_ADDRESS:
                        if ((c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9'))
                        {
                            update_checksum = true;
                        }
                        else
                        {
                            reset();
                        }
                    break;
                    case NMEA0183_FIELD_DATA_HEX_HI:
                        if (is_hex(c))
                        {
                            state = NMEA0183_FIELD_DATA_HEX_LO;
                        }
                        else
                        {
                            reset();
                        }
                    break;
                    case NMEA0183_FIELD_DATA_HEX_LO:
                        if (is_hex(c))
                        {
                            state = NMEA0183_FIELD_DATA;
                        }
                        else
                        {
                            reset();
                        }
                    break;
                    case NMEA0183_CHECKSUM_HI:
                        if (c == to_hex(checksum >> 4))
                        {
                            // checksum high nibble matches
                            state = NMEA0183_CHECKSUM_LO;
                        }
                        else
                        {
                            reset();
                        }
                    break;
                    case NMEA0183_CHECKSUM_LO:
                        if (c == to_hex(checksum & 0x0f))
                        {
                            // checksum valid
                            state = NMEA0183_EOS;
                        }
                        else
                        {
                            reset();
                        }
                    break;
                    case NMEA0183_INVALID:
                        // do nothing
                    break;
                    default:
                    break;
                };
            break;
        };
    }
    else if (state == NMEA0183_EOS && (c == 0x0a || c == 0x0d))
    {
        // NUL terminate
        c = 0;
        state = NMEA0183_ACCEPT;
    }
    else
    {
        // invalid
        reset();
    }

    if (state != NMEA0183_INVALID)
    {
        sentence[index++] = c;
        if (update_checksum)
        {
            checksum ^= c;
        }
        if ((index == sizeof(sentence) - 1) && state != NMEA0183_ACCEPT)
        {
            reset();
        }
    }

    return state == NMEA0183_ACCEPT;
}


const uint8_t NMEA0183::getState() const
{
    return state;
}


const uint8_t NMEA0183::getChecksum() const
{
    return checksum;
}


const char *NMEA0183::getSentence() const
{
    return state == NMEA0183_ACCEPT ? sentence : NULL;
}


const uint8_t NMEA0183::getFields() const
{
    return fields;
}
