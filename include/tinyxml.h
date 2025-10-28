#pragma once

class TiXmlNode {};
class TiXmlAttribute {};
class TiXmlText {};
class TiXmlElement {};
class TiXmlDocument {
public:
    bool LoadFile(const char*) { return true; }
    bool SaveFile(const char*) const { return true; }
};

class TiXmlVisitor {
public:
    virtual ~TiXmlVisitor() = default;
};
