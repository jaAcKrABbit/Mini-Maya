#version 150
uniform mat4 u_Model;
uniform mat4 u_ViewProj;

uniform mat4[50] u_OverallTransformMats;
uniform mat4[50] u_BindMats;

in vec4 vs_Pos;
in vec4 vs_Col;

in ivec2 vs_JointID;
in vec2 vs_Weight;

out vec4 fs_Col;

void main()
{
    fs_Col = vs_Col;
    vec4 j1Influence = u_OverallTransformMats[vs_JointID[0]] *
                        u_BindMats[vs_JointID[0]] * vs_Pos * vs_Weight[0];
    vec4 j2Influence = u_OverallTransformMats[vs_JointID[1]] *
            u_BindMats[vs_JointID[1]] * vs_Pos * vs_Weight[1];

    vec4 result = j1Influence + j2Influence;
    vec4 modelposition = u_Model * result;

    //built-in things to pass down the pipeline
    gl_Position = u_ViewProj * modelposition;

}

