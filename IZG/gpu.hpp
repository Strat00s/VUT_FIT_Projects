/*!
 * @file
 * @brief This file contains class that represents graphic card.
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 */
#pragma once

#include <student/fwd.hpp>
#include <unordered_map>
#include <vector>
#include <memory>


/**
 * @brief This class represent software GPU
 */
class GPU{
  public:
    GPU();
    virtual ~GPU();

    //buffer object commands
    BufferID  createBuffer           (uint64_t size);
    void      deleteBuffer           (BufferID buffer);
    void      setBufferData          (BufferID buffer,uint64_t offset,uint64_t size,void const* data);
    void      getBufferData          (BufferID buffer,uint64_t offset,uint64_t size,void      * data);
    bool      isBuffer               (BufferID buffer);

    //vertex array object commands (vertex puller)
    ObjectID  createVertexPuller     ();
    void      deleteVertexPuller     (VertexPullerID vao);
    void      setVertexPullerHead    (VertexPullerID vao,uint32_t head,AttributeType type,uint64_t stride,uint64_t offset,BufferID buffer);
    void      setVertexPullerIndexing(VertexPullerID vao,IndexType type,BufferID buffer);
    void      enableVertexPullerHead (VertexPullerID vao,uint32_t head);
    void      disableVertexPullerHead(VertexPullerID vao,uint32_t head);
    void      bindVertexPuller       (VertexPullerID vao);
    void      unbindVertexPuller     ();
    bool      isVertexPuller         (VertexPullerID vao);

    //program object commands
    ProgramID createProgram          ();
    void      deleteProgram          (ProgramID prg);
    void      attachShaders          (ProgramID prg,VertexShader vs,FragmentShader fs);
    void      setVS2FSType           (ProgramID prg,uint32_t attrib,AttributeType type);
    void      useProgram             (ProgramID prg);
    bool      isProgram              (ProgramID prg);
    void      programUniform1f       (ProgramID prg,uint32_t uniformId,float     const&d);
    void      programUniform2f       (ProgramID prg,uint32_t uniformId,glm::vec2 const&d);
    void      programUniform3f       (ProgramID prg,uint32_t uniformId,glm::vec3 const&d);
    void      programUniform4f       (ProgramID prg,uint32_t uniformId,glm::vec4 const&d);
    void      programUniformMatrix4f (ProgramID prg,uint32_t uniformId,glm::mat4 const&d);

    //framebuffer functions
    void      createFramebuffer      (uint32_t width,uint32_t height);
    void      deleteFramebuffer      ();
    void      resizeFramebuffer      (uint32_t width,uint32_t height);
    uint8_t*  getFramebufferColor    ();
    float*    getFramebufferDepth    ();
    uint32_t  getFramebufferWidth    ();
    uint32_t  getFramebufferHeight   ();



    //execution commands
    void      clear                  (float r,float g,float b,float a);
    void      drawTriangles          (uint32_t  nofVertices);
    void      interpol               (InFragment fragment_in, OutVertex A, OutVertex B, OutVertex C);
    float     scalar                 (std::pair<float, float> v0, std::pair<float, float> v1);

    /// \addtogroup gpu_init 00. proměnné, inicializace / deinicializace grafické karty
    /// @{
    /// \todo zde si můžete vytvořit proměnné grafické karty (buffery, programy, ...)
    /// @}

  private:

    //buffer table
    std::unordered_map<BufferID, std::vector<uint8_t>> buffer_table;  //IJC is useful - neusporadanej seznam, kde kazdy BufferID dostane vector(array) dat v bytech

    //1:16:00
    //head settings table
    struct puller_head{
      BufferID buffer;
      uint64_t offset;
      uint64_t stride;
      AttributeType type;
      bool enabled;
    };
    //puller settings
    struct puller_settings{
      BufferID buffer;
      IndexType buffer_type;
      std::array<puller_head, maxAttributes> heads;
    };
    ObjectID active_puller = -1;
    std::unordered_map<VertexPullerID, std::unique_ptr<puller_settings>> puller_table;

    //program settings
    struct program_settings{
      VertexShader vs;
      FragmentShader fs;
      Uniforms pr_uniforms;
      std::array<AttributeType, maxAttributes> frag_atributes;
    };
    ObjectID active_program = -1;
    std::unordered_map<ProgramID, std::unique_ptr<program_settings>> program_table;
    
    struct framebuffer{
      uint32_t width;
      uint32_t height;
      std::vector<uint8_t> color_data;
      std::vector<float> depth_data;
    } framebuffer;
};
