# Qt/C++ Conversion Notes

## Conversion Rules Applied

1. **Imports**: `#import` -> `#include`
2. **Base Classes**: `NSObject` -> `QObject`
3. **Strings**: `NSString` -> `QString`
4. **Collections**: `NSArray/NSMutableArray` -> `QList/QVector`
5. **Dictionaries**: `NSDictionary/NSMutableDictionary` -> `QMap`
6. **Classes**: `@interface/@implementation` -> `class`
7. **Nil**: `nil` -> `nullptr`
8. **Self**: `self` -> `this` (or removed for static methods)
9. **Method Calls**: `[obj method:param]` -> `obj->method(param)`
10. **Protocols**: Converted to abstract interface classes
11. **Data**: `NSData` -> `QByteArray`
12. **Sets**: `NSSet` -> `QSet`
13. **Memory Management**: `retain/release` removed (Qt uses parent-child)

## Files Converted

### Core Engine Files
- `TYScriptEngine.h/.cpp` - Main script engine (largest file)
- `TYScripterValues.h/.cpp` - Variable storage
- `TYArgment.h/.cpp` - Script argument handling
- `TYArgmentArray.h/.cpp` - Argument array parsing
- `TYArrayValue.h/.cpp` - Array variable handling
- `TYScriptStream.h/.cpp` - Script stream processing
- `TYScriptData.h/.cpp` - Script data management

### Foundation Files
- `TYSaveDataCoding.h/.cpp` - Save data interface
- `TYEnviroment.h/.cpp` - Environment settings
- `TYResourceServer.h/.cpp` - Resource loading
- `TYLoopValue.h/.cpp` - Loop control structures

### Proxy Files
- `TYCCSProxy.h/.cpp` - CCS proxy implementation
- `TYCCScripterProxy.h` - Scripter proxy interface

## Notes

- TYArgment.h/.m was not found in the source directory
- Some Cocoa-specific classes (NSImage, NSBundle, etc.) need Qt equivalents
- Memory management is simplified (Qt parent-child or smart pointers recommended)
- Selectors (SEL) are replaced with method names as strings or QMetaObject::invokeMethod
