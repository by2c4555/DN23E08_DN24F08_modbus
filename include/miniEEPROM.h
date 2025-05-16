#include <EEPROM.h>

class miniEEPROM {
private:
  uint16_t* _shadow;      // Shadow copy of EEPROM data
  uint16_t* _data;        // Pointer to user's array
  uint16_t _size;         // Number of elements
  uint16_t _eepromStart;  // Starting EEPROM address (in bytes)

public:
   // Constructor: Initializes shadow and loads data from EEPROM
  miniEEPROM(uint16_t* dataArray, uint16_t numElements, uint16_t startAddr) {
    _data = dataArray;
    _size = numElements;
    _eepromStart = startAddr;
    
    _shadow = new uint16_t[_size];
    load(); // Initialize data from EEPROM
  }

  // Destructor: Free shadow memory
  ~miniEEPROM() {
    delete[] _shadow;
  }

  // Load data from EEPROM (overwrites current values)
  void load() {
    for (uint16_t i = 0; i < _size; i++) {
      EEPROM.get(_eepromStart + (i * 2), _shadow[i]);
      _data[i] = _shadow[i]; // Sync user array with EEPROM
    }
  }

    // flush all changed values to EEPROM
  void flush() {
    for (uint16_t i = 0; i < _size; i++) {
      if (_data[i] != _shadow[i]) {
        _shadow[i] = _data[i]; // Update shadow
      }
    }
  }

  // Save all changed values to EEPROM
  void save() {
    for (uint16_t i = 0; i < _size; i++) {
      if (_data[i] != _shadow[i]) {
        EEPROM.put(_eepromStart + (i * 2), _data[i]);
        _shadow[i] = _data[i]; // Update shadow
      }
    }
  }
};