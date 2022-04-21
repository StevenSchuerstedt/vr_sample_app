#version 420 core

out vec4 FragColor;

in vec3 frag_pos;
in vec3 normal_pos;

uniform vec3 light_pos;

uniform vec3 view_pos;


void main(){

	//calculate lighting

	vec3 norm = normalize(normal_pos);
	vec3 lightDir = normalize(light_pos - frag_pos);
	float diff = max(dot(norm, lightDir), 0.0);

	vec3 diffuse = vec3(0.6f, 0.6f, 0.6f) * diff;

	vec3 ambient = vec3(0.1f, 0.1f, 0.1f);

	vec3 viewDir = normalize(view_pos - frag_pos);
	vec3 reflectDir = reflect(-lightDir, norm);

	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 8);
	vec3 specular = vec3(0.6f, 0.6f, 0.6) * spec; 

	vec3 result = (ambient + (diffuse + specular)) * glm::vec3(1.0f, 0.0f, 0.0f);
	
	FragColor = vec4(result, 1.0f);

}