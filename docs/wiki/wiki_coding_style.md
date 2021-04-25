# Coding Style

This page is under construction. Check [here](https://github.com/fiffty-50/openpilotlog/discussions/20) for the discussion.

See https://wiki.qt.io/Qt_Coding_Style for general code style guidelines.

|item|convention|example|
|----|----------|-------|
|Directory|All lowercase with `-` for word separator if necessary.|`src/subdirectory/directory-of-doom/`|
|File|All lowercase, follow Qts conventions.|`src/subdirectory/directory-of-doom/adatabase.h`|
|Class|PascalCase, follow Qt convention but change `Q` to `A`| `class ADataBase`|
|Methods|camelCase|`methodOfClass`
|Free Function|camelCase with an extra `a`|`aConquerTheWorld()`|
|Static Function|Without `a` and snake_case to emphasise limited scope|`void utility_function_one()`|
|Important variable|camelCase|`importantVariable`
|Limited scope variables|snake_case|`temporary_variable`|
|constants|All capital snake_case | `const auto IMPORTANT_CONSTANT = ASecretOfUniverse(42);`|
|Type Alias|follow Qt Convention|`using NewType = QMap<QString, int>`|


## Strings in openPilotLog

Aim to construct and use strings in the most efficient way. 

- For all user-facing string use [tr()](), so that the app is translatable.

- Use [QStringLiteral](https://doc-snapshots.qt.io/qt6-dev/qstring.html#QStringLiteral) where possible, except when using a function that has a `char *` overload.

- For string comparisons using the `==` operator, use [QLatin1String](https://wiki.qt.io/Using_QString_Effectively)

- For concatenations, openPilotLog uses [QStringBuilder](https://doc-snapshots.qt.io/qt6-dev/qstring.html#more-efficient-string-construction) globally, so the `+` operator can be used.
- (Debug etc. can be  `char *`, `QString` or anything else)

## Examples

```c++
const auto IMPORTANT_CONSTANT = ASecretOfUniverse(42);

APilotEntry(int throwaway_variable)
    : importantMemberVariable(throwaway_variable)
{}
```
```cpp
- AFooBar           - an important custom class.
- FooBar            - alias of important type.
- aFooBar()         - an important free function
- foo_bar()         - a static "utility" function
- niceVariable      - an important variable
- nice_variable     - a "consumable", limited scope variable.
```
```c++
class Foo {
    int rowId;
    Foo(int row_id) : rowId(row_id) {}
    void set(int parameter);
}

void Foo::set(int parameter)
{
   auto calc_flag = false;
   // complex calculations
   if(!calc_flag)
       return;
   ...
}
```