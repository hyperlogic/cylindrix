//
//  Shader.fsh
//  cylindrix
//
//  Created by Anthony Thibault on 5/4/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

varying lowp vec4 colorVarying;

void main()
{
    gl_FragColor = colorVarying;
}
