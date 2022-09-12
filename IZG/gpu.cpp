/*!
 * @file
 * @brief This file contains implementation of gpu
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 */

#include <student/gpu.hpp>
#include <iostream>


/// \addtogroup gpu_init
/// @{

/**
 * @brief Constructor of GPU
 */
GPU::GPU(){
  /// \todo Zde můžete alokovat/inicializovat potřebné proměnné grafické karty
}

/**
 * @brief Destructor of GPU
 */
GPU::~GPU(){
  /// \todo Zde můžete dealokovat/deinicializovat grafickou kartu
}

/// @}

/** \addtogroup buffer_tasks 01. Implementace obslužných funkcí pro buffery
 * @{
 */

//DONE
BufferID GPU::createBuffer(uint64_t size) { 
  auto buffer = std::vector<uint8_t>(size_t(size));   //just like array, but for c++
  BufferID id;
  id = reinterpret_cast<BufferID>(buffer.data());     //cast data pointer address as our id
  buffer_table.emplace(id, std::move(buffer));        //move our buffer (vector + id) to unordered map
  return id;
}
void GPU::deleteBuffer(BufferID buffer) {
  buffer_table.erase(buffer); //delete pair if it exists
}
void GPU::setBufferData(BufferID buffer, uint64_t offset, uint64_t size, void const* data) {
  auto iterator = buffer_table.find(buffer);      //try to find our buffer
  if (iterator != buffer_table.end()){            //if it does exist, copy the data to it
    auto from = (uint8_t*)data;                   //from where to get the data
    auto to = iterator->second.begin() + offset;  //where to save them - second item in item of the unordered map = data (id, vector(data));
    std::copy(from, from + size, to);             //copy from start, to end (so the size) and where to
  }
}
void GPU::getBufferData(BufferID buffer, uint64_t offset, uint64_t size, void*data) {
  auto iterator = buffer_table.find(buffer);          //try and find our buffer
  if (iterator != buffer_table.end()){                //if it does exist, copy to it data
    auto from = iterator->second.begin() + offset;  //from where to get the data
    auto to = (uint8_t*)data;                         //where to save them
    std::copy(from, from + size, to);                 //copy from start, to end (so the size) and where to
  }
}
bool GPU::isBuffer(BufferID buffer) { 
  auto iterator = buffer_table.find(buffer);
  //if pair does not exists, return false 
  if (iterator == buffer_table.end()){
    return false;
  }
  return true;
}


/// @}

/**
 * \addtogroup vertexpuller_tasks 02. Implementace obslužných funkcí pro vertex puller
 * @{
 */

ObjectID GPU::createVertexPuller (){
  auto puller = std::make_unique<puller_settings>();
  VertexPullerID id;
  id = reinterpret_cast<VertexPullerID>(puller.get());
  puller_table.emplace(id, std::move(puller));
  return id;
}
void GPU::deleteVertexPuller (VertexPullerID vao){
  puller_table.erase(vao);
}
void GPU::setVertexPullerHead (VertexPullerID vao,uint32_t head,AttributeType type,uint64_t stride,uint64_t offset,BufferID buffer){
  auto iterator = puller_table.find(vao);
  if (iterator != puller_table.end()){
    iterator->second->heads[head].type = type;
    iterator->second->heads[head].stride = stride;
    iterator->second->heads[head].offset = offset;
    iterator->second->heads[head].buffer = buffer;
    iterator->second->heads[head].enabled = false;
  }
}
void GPU::setVertexPullerIndexing(VertexPullerID vao,IndexType type,BufferID buffer){
  auto iterator = puller_table.find(vao);
  if (iterator != puller_table.end()){
    iterator->second->buffer_type = type;
    iterator->second->buffer = buffer;
  }
}
void GPU::enableVertexPullerHead (VertexPullerID vao,uint32_t head){
  auto iterator = puller_table.find(vao);
  if (iterator != puller_table.end()){
    iterator->second->heads[head].enabled = true;
  }
}
void GPU::disableVertexPullerHead(VertexPullerID vao,uint32_t head){
  auto iterator = puller_table.find(vao);
  if (iterator != puller_table.end()){
    iterator->second->heads[head].enabled = false;
  }
}
void GPU::bindVertexPuller (VertexPullerID vao){
  auto iterator = puller_table.find(vao);
  if (iterator != puller_table.end()){
    active_puller = iterator->first;
  }
}
void GPU::unbindVertexPuller (){
  active_puller = -1;
}
bool GPU::isVertexPuller (VertexPullerID vao){
    auto iterator = puller_table.find(vao);

  //if pair does not exists, return false 
  if (iterator == puller_table.end()){
    return false;
  }
  return true;
}

/// @}

/** \addtogroup program_tasks 03. Implementace obslužných funkcí pro shader programy
 * @{
 */

ProgramID GPU::createProgram (){
  auto program = std::make_unique<program_settings>();
  ProgramID id;
  id = reinterpret_cast<ProgramID>(program.get());
  program_table.emplace(id, std::move(program));
  return id;
}
void GPU::deleteProgram (ProgramID prg){
  program_table.erase(prg);
}
void GPU::attachShaders (ProgramID prg,VertexShader vs,FragmentShader fs){
  auto iterator = program_table.find(prg);
  if (iterator != program_table.end()){
    iterator->second->fs = fs;
    iterator->second->vs = vs;
  }
}
void GPU::setVS2FSType (ProgramID prg,uint32_t attrib,AttributeType type){
  auto iterator = program_table.find(prg);
  if (iterator != program_table.end()){
    iterator->second->frag_atributes[attrib] = type;
  }
}
void GPU::useProgram (ProgramID prg){
  auto iterator = program_table.find(prg);
  if (iterator != program_table.end()){
    active_program = iterator->first;
  }
}
bool GPU::isProgram (ProgramID prg){
  auto iterator = program_table.find(prg);
  if (iterator == program_table.end()){
    return false;
  }
  return true;
}

void GPU::programUniform1f (ProgramID prg,uint32_t uniformId,float const&d){
  auto iterator = program_table.find(prg);
  if (iterator != program_table.end()){
    iterator->second->pr_uniforms.uniform[uniformId].v1 = d;
  }
}
void GPU::programUniform2f (ProgramID prg,uint32_t uniformId,glm::vec2 const&d){
  auto iterator = program_table.find(prg);
  if (iterator != program_table.end()){
    iterator->second->pr_uniforms.uniform[uniformId].v2 = d;
  }
}
void GPU::programUniform3f (ProgramID prg,uint32_t uniformId,glm::vec3 const&d){
  auto iterator = program_table.find(prg);
  if (iterator != program_table.end()){
    iterator->second->pr_uniforms.uniform[uniformId].v3 = d;
  }
}
void GPU::programUniform4f (ProgramID prg,uint32_t uniformId,glm::vec4 const&d){
  auto iterator = program_table.find(prg);
  if (iterator != program_table.end()){
    iterator->second->pr_uniforms.uniform[uniformId].v4 = d;
  }
}
void GPU::programUniformMatrix4f(ProgramID prg,uint32_t uniformId,glm::mat4 const&d){
  auto iterator = program_table.find(prg);
  if (iterator != program_table.end()){
    iterator->second->pr_uniforms.uniform[uniformId].m4 = d;
  }
}

/// @}

/** \addtogroup framebuffer_tasks 04. Implementace obslužných funkcí pro framebuffer
 * @{
 */

void GPU::createFramebuffer (uint32_t width,uint32_t height){
  framebuffer.width = width;
  framebuffer.height = height;
  uint64_t size = width*height;     //one long vector, containing all data
  framebuffer.color_data.resize(size*4);  //(*4 for each color byte)
  framebuffer.depth_data.resize(size);
}
void GPU::deleteFramebuffer (){
  framebuffer.color_data.clear();
  framebuffer.depth_data.clear();

}
void GPU::resizeFramebuffer(uint32_t width,uint32_t height){
  framebuffer.width = width;
  framebuffer.height = height;
  uint64_t size = width*height;     //one long vector, containing all data
  framebuffer.color_data.resize(size*4);  //(*4 for each color byte)
  framebuffer.depth_data.resize(size);
}
uint8_t* GPU::getFramebufferColor (){
  return &framebuffer.color_data[0];
}
float* GPU::getFramebufferDepth (){
  return &framebuffer.depth_data[0];
}
uint32_t GPU::getFramebufferWidth (){
  return framebuffer.width;
}
uint32_t GPU::getFramebufferHeight (){
  return framebuffer.height;
}

/// @}

/** \addtogroup draw_tasks 05. Implementace vykreslovacích funkcí
 * Bližší informace jsou uvedeny na hlavní stránce dokumentace.
 * @{
 */

void GPU::clear (float r,float g,float b,float a){
  r = r*255.0;
  if (r > 255.0) r = 255.0;
  g = b = r;
  a = a*255.0;
  if (a > 255.0) a = 255.0;

  std::fill(framebuffer.color_data.begin(), framebuffer.color_data.end(), (uint8_t)r);
  for (unsigned int i = 1; i < framebuffer.width*framebuffer.height+1; i++){
    framebuffer.color_data[(i*4)-1] = (uint8_t)a;
  }
  std::fill(framebuffer.depth_data.begin(), framebuffer.depth_data.end(), std::numeric_limits<float>::infinity());
}



void GPU::drawTriangles (uint32_t  nofVertices){
  /// \todo Tato funkce vykreslí trojúhelníky podle daného nastavení.<br>
  /// Vrcholy se budou vybírat podle nastavení z aktivního vertex pulleru (pomocí bindVertexPuller).<br>
  /// Vertex shader a fragment shader se zvolí podle aktivního shader programu (pomocí useProgram).<br>
  /// Parametr "nofVertices" obsahuje počet vrcholů, který by se měl vykreslit (3 pro jeden trojúhelník).<br>

  auto iterator_prg = program_table.find(active_program);
  auto iterator_pul = puller_table.find(active_puller);

  InVertex vertex_in;

  std::vector<OutVertex> vertex_out_table{nofVertices};

  auto vex_id = 0;  //starting vertex id
  //vertex_in.gl_VertexID = 0;  //unique for every vertex shader

  auto puller_buf = buffer_table.find(iterator_pul->second->buffer);

  if (puller_buf != buffer_table.end()){

    auto idx = 0;
    
    // select which index we have
    switch (iterator_pul->second->buffer_type){
      case IndexType::UINT32: idx = 4; break;
      case IndexType::UINT16: idx = 2; break;
      case IndexType::UINT8 : idx = 1; break;
    }

    //go through the buffer (speed depends on indexing)
    auto iterator = puller_buf->second.begin();
    int j = 0;

    //while not at the end of the buffer
    while(iterator != puller_buf->second.end()){
      vertex_in.gl_VertexID = uint32_t(*iterator);        //

      //go through every head
      for (int i = 0; i < maxAttributes; i++){
        auto curr_head = iterator_pul->second->heads[i];  //save current head

        //if head is enabled
        if (curr_head.enabled){
          auto buffer_pos = vertex_in.gl_VertexID * curr_head.stride + curr_head.offset;  //which data to take
          auto head_buf = buffer_table.find(curr_head.buffer);                            //buffer iterator3

          //assing correct data to correct attribute
          switch (curr_head.type){
            case AttributeType::FLOAT: vertex_in.attributes[i].v1 = reinterpret_cast<float&>     (head_buf->second[buffer_pos]); break;
            case AttributeType::VEC2:  vertex_in.attributes[i].v2 = reinterpret_cast<glm::vec2&> (head_buf->second[buffer_pos]); break;
            case AttributeType::VEC3:  vertex_in.attributes[i].v3 = reinterpret_cast<glm::vec3&> (head_buf->second[buffer_pos]); break;
            case AttributeType::VEC4:  vertex_in.attributes[i].v4 = reinterpret_cast<glm::vec4&> (head_buf->second[buffer_pos]); break;
            default: break;
          }
        }
      }
      iterator_prg->second->vs(vertex_out_table[j], vertex_in, iterator_prg->second->pr_uniforms);  //call vertex sahder
      iterator += idx;                                                                              //jump depending on the indexing
      j++;                                                                                          //increase vertex_out_table index
    }
  }
  else{
    for (int i = 0; i < nofVertices; i++){
      vertex_in.gl_VertexID = vex_id; //assign id
      for (int j = 0; j < maxAttributes; j++){  //go through every head and find active one
        auto curr_head = iterator_pul->second->heads[j];  //assing head to curr_head
        if (curr_head.enabled == true){ //if enabled
          auto buffer_pos = vertex_in.gl_VertexID * curr_head.stride + curr_head.offset;  //which data to take
          auto iterator_buf = buffer_table.find(curr_head.buffer);                        //buffer iterator
          //assing correct data to correct attribute
          switch (curr_head.type){
            case AttributeType::FLOAT: vertex_in.attributes[j].v1 = reinterpret_cast<float&>     (iterator_buf->second[buffer_pos]); break;
            case AttributeType::VEC2:  vertex_in.attributes[j].v2 = reinterpret_cast<glm::vec2&> (iterator_buf->second[buffer_pos]); break;
            case AttributeType::VEC3:  vertex_in.attributes[j].v3 = reinterpret_cast<glm::vec3&> (iterator_buf->second[buffer_pos]); break;
            case AttributeType::VEC4:  vertex_in.attributes[j].v4 = reinterpret_cast<glm::vec4&> (iterator_buf->second[buffer_pos]); break;
            default: break;
          }
        }
      }
      iterator_prg->second->vs(vertex_out_table[i], vertex_in, iterator_prg->second->pr_uniforms); //call vertex shader
      vex_id++; //increase id for next vertex
    }
  }

  /*-------------------------------------------------------------------------------------------------------------------------------------------------*/

  //clipping

  //perspektivni deleni & viewport
  for (int i = 0; i < nofVertices; i++){
    if (vertex_out_table[i].gl_Position.w != 0.f){

      vertex_out_table[i].gl_Position.x /= vertex_out_table[i].gl_Position.w;
      vertex_out_table[i].gl_Position.y /= vertex_out_table[i].gl_Position.w;
      vertex_out_table[i].gl_Position.z /= vertex_out_table[i].gl_Position.w;

      vertex_out_table[i].gl_Position.x += 1.f;
      vertex_out_table[i].gl_Position.y += 1.f;

      vertex_out_table[i].gl_Position.x /= 2;
      vertex_out_table[i].gl_Position.y /= 2;

      vertex_out_table[i].gl_Position.x *= framebuffer.width;
      vertex_out_table[i].gl_Position.y *= framebuffer.height;
    }
  }
  

  //rasterizace
  auto pixel_cnt = framebuffer.height * framebuffer.width;
  
  InFragment fragment_in;
  std::vector<OutFragment> fragment_out_table{pixel_cnt/2};

  for (auto i = 0; i < nofVertices; i+=3){

    auto dABx = vertex_out_table[i+1].gl_Position.x - vertex_out_table[i].gl_Position.x;
    auto dABy = vertex_out_table[i+1].gl_Position.y - vertex_out_table[i].gl_Position.y;
    auto dBCx = vertex_out_table[i+2].gl_Position.x - vertex_out_table[i+1].gl_Position.x;
    auto dBCy = vertex_out_table[i+2].gl_Position.y - vertex_out_table[i+1].gl_Position.y;
    auto dCAx = vertex_out_table[i].gl_Position.x - vertex_out_table[i+2].gl_Position.x;
    auto dCAy = vertex_out_table[i].gl_Position.y - vertex_out_table[i+2].gl_Position.y;
    
    for (int y = 0; y < framebuffer.height; y++){
      for (int x = 0; x < framebuffer.width; x++){
        auto eAB = ((x + 0.5f) - vertex_out_table[i].gl_Position.x)*dABy - ((y + 0.5f) - vertex_out_table[i].gl_Position.y)*dABx;
        auto eBC = ((x + 0.5f) - vertex_out_table[i].gl_Position.x)*dBCy - ((y + 0.5f) - vertex_out_table[i].gl_Position.y)*dBCx;
        auto eCA = ((x + 0.5f) - vertex_out_table[i].gl_Position.x)*dCAy - ((y + 0.5f) - vertex_out_table[i].gl_Position.y)*dCAx;
        if ((eAB >= 0 && eBC >= 0 && eCA >= 0) || (eAB < 0 && eBC < 0 && eCA < 0)){
          fragment_in.gl_FragCoord.x = x + 0.5f;
          fragment_in.gl_FragCoord.y = y + 0.5f;
          
          //interpol(fragment_in, vertex_out_table[i], vertex_out_table[i+1], vertex_out_table[i+2]);
          iterator_prg->second->fs(fragment_out_table[i], fragment_in, iterator_prg->second->pr_uniforms);
        }
      }
    }
  }
}

float GPU::scalar(std::pair<float, float> v0, std::pair<float, float> v1){
  return v0.first * v1.first + v0.second * v1.second;
}

//interpolace (not done)
void GPU::interpol(InFragment fragment_in, OutVertex A, OutVertex B, OutVertex C){
  //source: https://gamedev.stackexchange.com/a/23745
  std::pair<float, float> v0, v1, v2;
  float sc00 = scalar(v0, v0);
  float sc01 = scalar(v0, v1);
  float sc11 = scalar(v1, v1);
  float sc20 = scalar(v2, v0);
  float sc21 = scalar(v2, v1);
}
/// @}
