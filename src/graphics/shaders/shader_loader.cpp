#include <graphics/shaders/shader_loader.h>

namespace Engine {

/*
 * Class ShaderObject
 */
ShaderObject::ShaderObject(const GLenum type, const std::string filePath) : ShaderObject(type) {
    load(filePath);
}

ShaderObject::~ShaderObject() {
    release();
}

void ShaderObject::load(const std::string filePath) {
    this->filePath = filePath;
    std::string source;
    readFile(filePath, source);
    const char* sourceCString = source.c_str();
    shader = glCreateShader(type);
    if(!glIsShader(shader)) {
        throw RenderException("ERROR: Failed to create shader object.");
    }
    glShaderSource(shader, 1, &sourceCString, NULL);
}

void ShaderObject::compile() {
    if(!shader) {
        throw RenderException("ERROR: Attempted to compile shader file \"" + filePath + "\"that wasn't loaded.");
    }
    glCompileShader(shader);
    int compleStatus = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compleStatus);
    if(compleStatus != GL_TRUE) {
        char infoLog[1000] = {0};
        int infoLogLength = 0;
        glGetShaderInfoLog(shader, 1000, &infoLogLength, infoLog);
        throw RenderException("ERROR: Failed to compile shader file \"" + filePath + "\". InfoLog:\n" + std::string(infoLog, infoLogLength));
    }
}

void ShaderObject::release() {
    if(shader != 0) {
        glDeleteShader(shader);
    }
    shader = 0;
    filePath = "";
}

/*
 * Class ShaderProgram
 */
ShaderProgram::ShaderProgram(const std::vector<GLenum> types, const std::vector<std::string> filePaths, const std::string shaderProgramName) : ShaderProgram() {
#ifdef _DEBUG
    assert(types.size() == filePaths.size());
#endif
    create();
    this->shaderProgramName = shaderProgramName;
    std::vector<std::shared_ptr<ShaderObject>> shaderObjects;
    for(size_t i = 0; i < filePaths.size(); i++) {
        std::shared_ptr<ShaderObject> shaderObject(new ShaderObject(types[i], filePaths[i]));
        shaderObject->compile();
        shaderObjects.push_back(shaderObject);
    }
    for(size_t i = 0; i < filePaths.size(); i++) {
        addShaderObject(shaderObjects[i]);
    }
    link();
}

ShaderProgram::~ShaderProgram() {
    release();
}

ShaderProgram& ShaderProgram::operator=(const ShaderProgram& shaderProgram) {
    release();
    program = shaderProgram.program;
    linked = shaderProgram.linked;
    shaderFileNames = shaderProgram.shaderFileNames;
    return (*this);
}

void ShaderProgram::create() {
    program = glCreateProgram();
    if(!glIsProgram(program)) {
        throw RenderException("ERROR: Failed to create shader program.");
    }
}

/*
 * Assumes shaderObject has already been compiled successfully.
 */
void ShaderProgram::addShaderObject(const std::shared_ptr<ShaderObject> shaderObject) {
    if(!program) {
        throw RenderException("ERROR: Attempted to add shader object \"" + shaderObject->getFileName() + "\" to shader program before it was created.");
    }
    glAttachShader(program, shaderObject->getShader());
    shaderFileNames.push_back(shaderObject->getFileName());
    linked = false;
}

void ShaderProgram::detachShaderObject(const std::shared_ptr<ShaderObject> shaderObject) {
    if(!program) {
        throw RenderException("ERROR: Attempted to add shader object \"" + shaderObject->getFileName() + "\" to shader program before the program was created.");
    }
    glDetachShader(program, shaderObject->getShader());
}

void ShaderProgram::link() {
    if(!program) {
        throw RenderException("ERROR: Attempted to link shader program that wasn't created.");
    }
    glLinkProgram(program);
    int linkStatus = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
    if(linkStatus != GL_TRUE) {
        char infoLog[1000] = {0};
        int infoLogLength = 0;
        glGetProgramInfoLog(program, 1000, &infoLogLength, infoLog);
        std::string errMessage = std::string("ERROR: Failed to link shader program with shader objects ");
        for(std::vector<std::string>::iterator shaderFileName = shaderFileNames.begin(); shaderFileName != shaderFileNames.end(); shaderFileName++) {
            errMessage = errMessage + "\"" + (*shaderFileName) + "\" ";
        }
        errMessage = errMessage + "\nInfoLog:\n" + std::string(infoLog, infoLogLength);
        throw RenderException(errMessage);
    }
    linked = true;
}

void ShaderProgram::release() {
    if(program != 0) {
        glDeleteProgram(program);
    }
    program = 0;
    linked = false;
    while(shaderFileNames.size() > 0) {
        shaderFileNames.pop_back();
    }
}

void ShaderProgram::use() const {
    if(!program) {
        throw RenderException("ERROR: Attempted to use shader program that wasn't created.");
    }
    if(!linked) {
        throw RenderException("ERROR: Attempted to use shader program that linked.");
    }
    glUseProgram(program);
}

/*
 * Class ShaderLoader
 */
std::vector<ShaderProgramPtr> ShaderLoader::loadedShaderPrograms = std::vector<ShaderProgramPtr>();

void ShaderLoader::LoadShaderPrograms(const std::vector<ShaderFiles>& shaderFiles) {
#ifdef _DEBUG
    assert(shaderFiles.size() > 0);
#endif
    for(unsigned int i = 0; i < shaderFiles.size(); i++) {
        std::vector<GLenum> types;
        std::vector<std::string> filePaths;
#ifdef _DEBUG
        assert(shaderFiles[i].shaderProgramName != "");
        assert(shaderFiles[i].vertexShaderFilePath != "");
        assert(shaderFiles[i].fragmentShaderFilePath != "");
#endif
        types.push_back(GL_VERTEX_SHADER);
        types.push_back(GL_FRAGMENT_SHADER);
        filePaths.push_back(shaderFiles[i].vertexShaderFilePath);
        filePaths.push_back(shaderFiles[i].fragmentShaderFilePath);
        if(shaderFiles[i].geometryShaderFilePath != "") {
            types.push_back(GL_GEOMETRY_SHADER);
            filePaths.push_back(shaderFiles[i].geometryShaderFilePath);
        }
        ShaderProgramPtr shaderProgramPtr = std::make_shared<ShaderProgram>(types, filePaths, shaderFiles[i].shaderProgramName);
        loadedShaderPrograms.push_back(shaderProgramPtr);
    }
}

ShaderProgramPtr ShaderLoader::getShaderProgram(const std::string& shaderProgramName) {
    int index = -1;
    for(unsigned int i = 0; i < loadedShaderPrograms.size(); i++) {
        if(shaderProgramName == loadedShaderPrograms[i]->getShaderProgramName()) {
            index = i;
            break;
        }
    }
#ifdef _DEBUG
    assert(index != -1);
#endif
    return loadedShaderPrograms[index];
}

};
