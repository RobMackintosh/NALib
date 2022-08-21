
#include "../NAAppCore.h"

#if NA_COMPILE_GUI == 1

#include "../../../NAMath/NAVectorAlgebra.h"



NA_HDEF void na_InitOpenGLSpace(NAOpenGLSpace* openGLSpace, void* nativePtr){
  na_InitUIElement(&(openGLSpace->uiElement), NA_UI_OPENGL_SPACE, nativePtr);
}



NA_HDEF void na_ClearOpenGLSpace(NAOpenGLSpace* openGLSpace){
  na_ClearUIElement(&(openGLSpace->uiElement));
}



NAByte na_pixelFont5x9[(5*16/8)*9*6] = {
  0x01, 0x14, 0xa7, 0xe5, 0x84, 0x12, 0x00, 0x00, 0x00, 0x01, 
  0x01, 0x15, 0xfa, 0x6a, 0x44, 0x21, 0x00, 0x00, 0x00, 0x02, 
  0x01, 0x00, 0xaa, 0x0a, 0x40, 0x40, 0x94, 0x40, 0x00, 0x02, 
  0x01, 0x00, 0xa7, 0x11, 0x80, 0x40, 0x88, 0xe0, 0x38, 0x04, 
  0x01, 0x00, 0xa2, 0xa2, 0x60, 0x40, 0x94, 0x40, 0x00, 0x08, 
  0x00, 0x01, 0xf2, 0xae, 0x40, 0x40, 0x80, 0x00, 0x00, 0x08, 
  0x01, 0x00, 0xaf, 0x4d, 0xa0, 0x21, 0x00, 0x02, 0x00, 0x90, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x12, 0x00, 0x02, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 
  0x71, 0x1c, 0xe1, 0x7c, 0xdf, 0x73, 0x80, 0x00, 0x00, 0x0e, 
  0x8b, 0x23, 0x19, 0x41, 0x01, 0x8c, 0x40, 0x01, 0x01, 0x11, 
  0x9d, 0x02, 0x19, 0x42, 0x02, 0x8c, 0x40, 0x02, 0x38, 0x81, 
  0xa9, 0x0c, 0x2f, 0xfb, 0xc4, 0x73, 0xc8, 0x44, 0x00, 0x42, 
  0xc9, 0x10, 0x11, 0x06, 0x24, 0x88, 0x40, 0x02, 0x38, 0x84, 
  0x89, 0x21, 0x11, 0x06, 0x24, 0x88, 0x40, 0x01, 0x01, 0x00, 
  0x77, 0xfe, 0xe1, 0x79, 0xc4, 0x73, 0x88, 0x40, 0x00, 0x04, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 
  0x71, 0x3c, 0xef, 0x7f, 0xef, 0x8f, 0xc3, 0x18, 0x46, 0x2e, 
  0x8a, 0xa3, 0x18, 0xc2, 0x10, 0x89, 0x03, 0x28, 0x6f, 0x31, 
  0x9c, 0x63, 0x08, 0xc2, 0x10, 0x89, 0x03, 0x48, 0x56, 0xb1, 
  0xaf, 0xfd, 0x08, 0xf3, 0x93, 0xf9, 0x03, 0x88, 0x46, 0x71, 
  0xbc, 0x63, 0x08, 0xc2, 0x11, 0x89, 0x03, 0x48, 0x46, 0x31, 
  0x84, 0x63, 0x18, 0xc2, 0x11, 0x89, 0x23, 0x28, 0x46, 0x31, 
  0x74, 0x7c, 0xef, 0x7e, 0x0f, 0x8f, 0xdd, 0x1f, 0xc6, 0x2e, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0xf3, 0xbc, 0xff, 0xc6, 0x31, 0x8c, 0x7e, 0xe8, 0x38, 0x80, 
  0x8c, 0x63, 0x02, 0x46, 0x31, 0x54, 0x42, 0x84, 0x09, 0x40, 
  0x8c, 0x63, 0x02, 0x46, 0x31, 0x52, 0x84, 0x84, 0x08, 0x00, 
  0xf4, 0x7c, 0xe2, 0x46, 0x31, 0x21, 0x08, 0x82, 0x08, 0x00, 
  0x85, 0x68, 0x12, 0x46, 0x35, 0x51, 0x10, 0x81, 0x08, 0x00, 
  0x84, 0xa4, 0x12, 0x45, 0x55, 0x51, 0x20, 0x81, 0x08, 0x00, 
  0x83, 0x63, 0xe2, 0x3c, 0x9b, 0x89, 0x3e, 0xe0, 0xb8, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 
  0x40, 0x20, 0x00, 0x80, 0x40, 0x81, 0x09, 0x04, 0x00, 0x00, 
  0x20, 0x20, 0x00, 0x80, 0xa0, 0x80, 0x01, 0x04, 0x00, 0x00, 
  0x13, 0xbc, 0xf7, 0xb8, 0x8f, 0xf3, 0x09, 0x14, 0x6b, 0xce, 
  0x00, 0x63, 0x08, 0xc5, 0xd1, 0x89, 0x09, 0x24, 0x56, 0x31, 
  0x03, 0xe3, 0x08, 0xfc, 0x91, 0x89, 0x09, 0xc4, 0x56, 0x31, 
  0x04, 0x63, 0x08, 0xc0, 0x91, 0x89, 0x09, 0x24, 0x56, 0x31, 
  0x03, 0xfc, 0xf7, 0xb8, 0x8f, 0x8b, 0x89, 0x13, 0x56, 0x2e, 
  0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x08, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x0e, 0x00, 0x30, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x32, 0x60, 0x00, 
  0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x42, 0x10, 0x00, 
  0xf3, 0xd6, 0xf7, 0x46, 0x31, 0x8c, 0x7e, 0x42, 0x11, 0x00, 
  0x8c, 0x59, 0x02, 0x46, 0x31, 0x54, 0x44, 0x82, 0x0a, 0xa0, 
  0x8c, 0x50, 0xe2, 0x46, 0x31, 0x24, 0x48, 0x42, 0x10, 0x40, 
  0x8c, 0x50, 0x12, 0x45, 0x55, 0x54, 0x50, 0x42, 0x10, 0x00, 
  0xf3, 0xd1, 0xe1, 0xbc, 0x8a, 0x8b, 0xfe, 0x32, 0x60, 0x00, 
  0x80, 0x40, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 
  0x80, 0x40, 0x00, 0x00, 0x00, 0x07, 0x80, 0x00, 0x00, 0x00, 
};


#if NA_COMPILE_OPENGL == 1

  NA_DEF NAInt naStartupPixelFont(){
    GLuint fontTex = 0;
    NAByte texBuf[5*16 * 9*6 * 4];

    NAByte* byte = na_pixelFont5x9;
    NAByte* texPtr = texBuf;
    
    int byteNum = 7;
    for(int y = 0; y < 9*6; ++y){
      for(int x = 0; x < 5*16; ++x){
        NAByte value = (*byte >> byteNum) & 0x01;
        *texPtr++ = 0xff * value;
        *texPtr++ = 0xff * value;
        *texPtr++ = 0xff * value;
        *texPtr++ = 0xff * value;
        
        --byteNum;
        if(byteNum == -1){
          byteNum = 7;
          ++byte;
        }
      }
    }

    glGenTextures(1, &fontTex);
    glBindTexture(GL_TEXTURE_2D, fontTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 5*16, 9*6, 0, GL_RGBA, GL_UNSIGNED_BYTE, texBuf);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glBindTexture(GL_TEXTURE_2D, 0);
    return (NAInt)fontTex;
  }



  NA_DEF void naShutdownPixelFont(NAInt fontId){
    GLuint fontTex = (GLuint)fontId;
    glDeleteTextures(1, &fontTex);
  }


  NA_DEF void naDrawASCIICharacters(NAInt fontId, const char* str, double x, double y, double z){
    GLuint fontTex = (GLuint)fontId;
    NAMat44d modelViewMatrix;
    NAMat44d invModelViewMatrix;
    NAMat44d projectionMatrix;
    NAMat44d invProjectionMatrix;
    glGetDoublev(GL_MODELVIEW_MATRIX, modelViewMatrix);
    naInvM44d(invModelViewMatrix, modelViewMatrix);
    glGetDoublev(GL_PROJECTION_MATRIX, projectionMatrix);
    naInvM44d(invProjectionMatrix, projectionMatrix);

    NAVec4d viewPort;
    glGetDoublev(GL_VIEWPORT, viewPort);
    NAVec4d pos = {x, y, z, 1.};
    NAVec4d modelPos;
    NAVec4d projPos;
    NAVec2d normPos;
    naMulM44dV4d(modelPos, modelViewMatrix, pos);
    naMulM44dV4d(projPos, projectionMatrix, modelPos);
    
    if(!naAlmostZero(projPos[3]))
    {
      naDivV2d(normPos, projPos, projPos[3]);
      int viewX = (int)((normPos[0] + 1.) * .5 * viewPort[2]);
      int viewY = (int)((normPos[1] + 1.) * .5 * viewPort[3]);

      glMatrixMode(GL_MODELVIEW);
      glPushMatrix();
      glLoadIdentity();
      glMatrixMode(GL_PROJECTION);
      glPushMatrix();
      glLoadIdentity();
      glOrtho(0, viewPort[2], 0, viewPort[3], -1., 1.);

      glEnable(GL_TEXTURE_2D);
      glBindTexture(GL_TEXTURE_2D, fontTex);
      glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

      int offsetX = 0;
      for(size_t i = 0; i < strlen(str); ++i){
        char c = str[i];
        
        if(c == '\n'){
          viewY -= 10;
          offsetX = 0;
        }
        if(c < 32 || c > 127)
          continue;

        float texX = (float)(c % 16);
        float texY = (float)(c / 16 - 2);
        float scaleX = 1.f / (5.f * 16.f);
        float scaleY = 1.f / (6.f * 9.f);
        
        glBegin(GL_TRIANGLE_STRIP);
          glTexCoord2f((texX * 5.f) * scaleX, ((texY + 1.f) * 9.f) * scaleY);
          glVertex3i(viewX + offsetX, viewY, 0);
          glTexCoord2f((texX * 5.f) * scaleX, (texY * 9.f) * scaleY);
          glVertex3i(viewX + offsetX, viewY + 9, 0);
          glTexCoord2f(((texX + 1.f) * 5.f) * scaleX, ((texY + 1.f) * 9.f) * scaleY);
          glVertex3i(viewX + offsetX + 5, viewY, 0);
          glTexCoord2f(((texX + 1.f) * 5.f) * scaleX, (texY * 9.f) * scaleY);
          glVertex3i(viewX + offsetX + 5, viewY + 9, 0);
        glEnd();
        
        offsetX += 6;
      }

      glPopMatrix();
      glMatrixMode(GL_MODELVIEW);
      glPopMatrix();

      glBindTexture(GL_TEXTURE_2D, 0); 
    }
  }

#else

  NA_API void naStartupPixelFont(void){
    #if NA_DEBUG
      naError("Function has no effect when NA_COMPILE_OPENGL is undefined.");
    #endif
  }
  NA_API void naShutdownPixelFont(void){
    #if NA_DEBUG
      naError("Function has no effect when NA_COMPILE_OPENGL is undefined.");
    #endif
  }
  NA_API void naDrawASCIICharacter(char c, double x, double y, double z){
    #if NA_DEBUG
      naError("Function has no effect when NA_COMPILE_OPENGL is undefined.");
    #endif
  }

#endif // NA_COMPILE_OPENGL == 1


#endif // NA_COMPILE_GUI == 1
