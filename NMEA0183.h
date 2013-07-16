#ifndef NMEA0183_H
#define NMEA0183_H 1

#ifdef ARDUINO
# if (ARDUINO >= 100)
#  include <Arduino.h>
# else
#  include <WProgram.h>
# endif
#else
# include <stdint.h>
#endif

enum
{
    NMEA0183_INVALID = 0,
    NMEA0183_ADDRESS,
    NMEA0183_FIELD_DATA,
    NMEA0183_FIELD_DATA_HEX_HI,
    NMEA0183_FIELD_DATA_HEX_LO,
    NMEA0183_CHECKSUM_HI,
    NMEA0183_CHECKSUM_LO,
    NMEA0183_EOS,
    NMEA0183_ACCEPT
};

class NMEA0183
{
public:
    NMEA0183();
    void reset();
    bool update(char c);
    const char *getSentence() const;
    const uint8_t getFields() const;

    const uint8_t getState() const;
    const uint8_t getChecksum() const;

protected:

private:
    /// Current state
    uint8_t state;
    /// Checksum update flag
    bool update_checksum;
    /// NMEA0183 sentence buffer.
    // The maximum number of characters in a sentence shall be 82, consisting
    // of a maximum of 79 characters between the starting delimiter "$" or "!"
    // and the terminating <CR><LF>.
    char sentence[82 + 1 /* NUL terminator */];
    /// Current index within @p sentence
    uint8_t index;
    /// Number of fields in current sentence
    uint8_t fields;
    /// Sentence checksum
    uint8_t checksum;
};

#endif /* NMEA0183_H */
