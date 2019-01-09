# 2. Property Naming

In addition to complying to the json property name syntax,  

- We will use camel notation to name all properties and configuration groups. 
- Use underscores in place of dashes or dots when converting names from other domains to property names. 
- Use `__` prefix for meta properties. Meta properties are those intended for the device client and not for the device state. For example, `__version` is a meta property describing the json schema version for the client.

----

[Schema](../schema.md)