in vec3 lightDir;
in vec3 viewDir;
in vec2 texCoord;
in vec3 position;

uniform sampler2D texture0; // Diffuse
uniform sampler2D texture1; // Normal

struct LightInfo
{
    vec4 position;  // Light position in eye coords.
    vec3 intensity; // A,D,S intensity
    float ac;       // Constant attenuation
    float al;       // Linear attenuation
    float aq;       // Quadratic attenuation
};
uniform LightInfo light;

struct MaterialInfo
{
    vec3 Ka;            // Ambient reflectivity
    vec3 Ks;            // Specular reflectivity
    float shininess;    // Specular shininess factor
};
uniform MaterialInfo material;

layout( location = 0 ) out vec4 fragColor;

void adsModel( const in vec3 norm, const in vec3 diffuseReflect, out vec3 ambientAndDiff, out vec3 spec )
{
    // Reflection of light direction about normal
    vec3 r = reflect( -lightDir, norm );

    // Calculate distance from the fragment to the light
    float dist = distance( vec3( light.position ), position );

    // Calculate light intensity when attenuation is applied
    vec3 intensity = light.intensity / ( light.aq * pow( dist, 2.0 ) + light.al * dist + light.ac );

    // Calculate the ambient contribution
    vec3 ambient = intensity * material.Ka;

    // Calculate the diffuse contribution
    float sDotN = max( dot( lightDir, norm ), 0.0 );
    vec3 diffuse = intensity * diffuseReflect * sDotN;

    // Sum the ambient and diffuse contributions
    ambientAndDiff = ambient + diffuse;

    // Calculate the specular highlight contribution
    spec = vec3( 0.0 );
    if ( sDotN > 0.0 )
        spec = intensity * pow( max( dot( r, viewDir ), 0.0 ), material.shininess ) * ( material.shininess + 2.0 ) / 2.0;
}

void main()
{
    // Sample the textures at the interpolated texCoords
    vec4 diffuseTextureColor = texture( texture0, texCoord );

    // We have to scale and bias the normal data
    vec4 normal = 2.0 * texture( texture1, texCoord ) - vec4( 1.0 );

    // Calculate the lighting model, keeping the specular component separate
    vec3 ambientAndDiff, spec;
    adsModel( normalize( normal.xyz ), diffuseTextureColor.xyz, ambientAndDiff, spec );

    // Add specular contribution for final fragment color
    fragColor = vec4( ambientAndDiff + spec, 1.0 );
}
