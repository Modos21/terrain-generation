/**
 * Einige ältere Funktionen können zwar noch genutzt werden, wurden aber effektiv durch neuere ersetzt.
 * Um ein vermischen dieser Funktionen (und damit schwer zu findende Fehler) zu verhindern, werden
 * die alten Funktionen hier deaktiviert.
 * 
 * @copyright 2020-2026, FH Wedel
 * @authors Philipp Munz
 */

#ifndef FHWCG_DISABLED_H
#define FHWCG_DISABLED_H

#undef glGenBuffers
#undef glBindBuffer
#undef glBufferData
#undef glBufferSubData
#undef glGetBufferSubData
#undef glMapBuffer
#undef glMapBufferRange
#undef glUnmapBuffer
#undef glGetBufferParameteriv
#undef glGetBufferParameteri64v
#undef glGetBufferPointerv

#undef glGenTextures
#undef glActiveTexture
#undef glBindTexture
#undef glTexImage1D
#undef glTexImage2D
#undef glTexImage3D
#undef glTexSubImage1D
#undef glTexSubImage2D
#undef glTexSubImage3D
#undef glCompressedTexImage1D
#undef glCompressedTexImage2D
#undef glCompressedTexImage3D
#undef glCompressedTexSubImage1D
#undef glCompressedTexSubImage2D
#undef glCompressedTexSubImage3D
#undef glGenerateMipmap
#undef glTexParameteri
#undef glTexParameterf
#undef glTexParameteriv
#undef glTexParameterfv
#undef glGetTexParameteriv
#undef glGetTexParameterfv
#undef glGetTexLevelParameteriv
#undef glGetTexLevelParameterfv
#undef glGetTexImage

#undef glGenFramebuffers
#undef glFramebufferTexture
#undef glFramebufferTexture1D
#undef glFramebufferTexture2D
#undef glFramebufferTexture3D
#undef glFramebufferTextureLayer
#undef glFramebufferRenderbuffer
#undef glCheckFramebufferStatus
#undef glGetFramebufferAttachmentParameteriv

#undef glGenRenderbuffers
#undef glBindRenderbuffer
#undef glRenderbufferStorage
#undef glRenderbufferStorageMultisample
#undef glGetRenderbufferParameteriv

#undef glGenVertexArrays
#undef glVertexAttribPointer
#undef glVertexAttribIPointer
#undef glVertexAttribLPointer
#undef glEnableVertexAttribArray
#undef glDisableVertexAttribArray
#undef glVertexAttribDivisor

// Es gibt eigentlich noch eine ganze Menge mehr aber das sind zumindest die wichtigsten.
#undef glUniform1f
#undef glUniform2f
#undef glUniform3f
#undef glUniform4f
#undef glUniform1i
#undef glUniform2i
#undef glUniform3i
#undef glUniform4i
#undef glUniform1ui
#undef glUniform2ui
#undef glUniform3ui
#undef glUniform4ui
#undef glUniform1fv
#undef glUniform2fv
#undef glUniform3fv
#undef glUniform4fv
#undef glUniform1iv
#undef glUniform2iv
#undef glUniform3iv
#undef glUniform4iv
#undef glUniform1uiv
#undef glUniform2uiv
#undef glUniform3uiv
#undef glUniform4uiv
#undef glUniformMatrix3fv
#undef glUniformMatrix4fv

#endif // FHWCG_DISABLED_H
