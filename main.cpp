#include <string>
#include <vector>

#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/allocators.h>

class Buffer {
private:
    uint8_t*    _buffer;
    size_t      _length;
public:
    Buffer()
        : _buffer(nullptr)
        , _length(0)
    {}
    uint8_t const* buffer() const {
        return _buffer;
    }
    size_t length() const {
        return _length;
    }
    ~Buffer() {
        if(_buffer) {
            free(_buffer);
        }
    }
    static Buffer* createFileBuffer( char const* filepath) {
        auto file = fopen(filepath, "rb");
        if(!file) {
            return nullptr;
        }
        auto set = ftell(file);
        fseek(file, 0, SEEK_END);
        auto length = ftell(file) - set;
        Buffer* buffer = new Buffer();
        buffer->_buffer = (uint8_t*)malloc(length + 1);
        buffer->_length = length;
        fseek(file, 0, SEEK_SET);
        fread(buffer->_buffer, 1, buffer->_length, file);
        buffer->_buffer[buffer->_length] = 0;
        fclose(file);
        return buffer;
    }
};

int main(int argc, char const* const* argv) {
    std::string rootpath = argv[0];
    auto pos = rootpath.find_last_of("/\\");
    if(pos == std::string::npos) {
        return -1;
    }
    rootpath.resize(pos+1);
    std::string propsJsonPath ;
    if(argc == 1) {
        propsJsonPath = rootpath + ".vscode\\c_cpp_properties.json";
    } else {
        propsJsonPath = std::string(argv[1]) + "\\.vscode\\c_cpp_properties.json";
    }
    auto buff = Buffer::createFileBuffer(propsJsonPath.c_str());
    rapidjson::Document propsDoc;
    propsDoc.Parse((char const*)buff->buffer());
    delete buff;

    auto& configurations = propsDoc.GetObject()["configurations"].GetArray();
    for(auto const& prop:configurations) {
        buff = Buffer::createFileBuffer(prop["compileCommands"].GetString());
        rapidjson::Document commandsDoc;
        commandsDoc.Parse((char const*)buff->buffer(), buff->length());
        auto arr = commandsDoc.GetArray();
        bool needRepair = false;
        for(auto iter = arr.begin(); iter!=arr.end(); ++iter) {
            auto obj = iter->GetObject();
            auto cmd = obj.FindMember("command");
            if(cmd != obj.end()) {
                auto str = (*cmd).value.GetString();
                if(str[0] == '\\') {
                    needRepair = true;
                    auto pos = strstr(str, "@");
                    std::string recomposed = "\"" + std::string(str, pos - str -1) + "\"" + (pos - 1);
                    cmd->value.SetString(recomposed.c_str(), recomposed.size(), commandsDoc.GetAllocator());
                }
            }
        }
        if(needRepair) {
            rapidjson::StringBuffer buffer;  
            rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);  
            commandsDoc.Accept(writer);
            delete buff;
            auto file = fopen(prop["compileCommands"].GetString(), "wb");
            fwrite(buffer.GetString(), 1, buffer.GetLength(), file);
            fclose(file);
        }
    }
    printf("repair complete!");
    system("pause");
    return 0;
}