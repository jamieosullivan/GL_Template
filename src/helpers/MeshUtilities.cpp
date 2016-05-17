#include "MeshUtilities.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstddef>
#include <map>

using namespace std;

void loadObj(const std::string & filename, mesh_t & mesh, LoadMode mode){
	// Open the file.
	ifstream in;
	in.open(filename.c_str());
	if (!in) {
		cerr << filename + " is not a valid file." << endl;
		return;
	}
	//Init the mesh.
	mesh.indices.clear();
	mesh.positions.clear();
	mesh.normals.clear();
	mesh.texcoords.clear();
	// Init temporary vectors.
	vector<glm::vec3> positions_temp;
	vector<glm::vec3> normals_temp;
	vector<glm::vec2> texcoords_temp;
	vector<string> faces_temp;

	string res;

	// Iterate over the lines of the file.
	while(!in.eof()){
		getline(in,res);

		// Ignore the line if it is too short or a comment.
		if(res.empty() || res[0] == '#' || res.size()<2){
			continue;
		}
		//We want to split the content of the line at spaces, use a stringstream directly
		stringstream ss(res);
		vector<string> tokens;
		string token;
		while(ss >> token){
			tokens.push_back(token);
		}
		if(tokens.size() < 1){
			continue;
		}
		// Check what kind of element the line represent.
		if (tokens[0] == "v") { // Vertex position
			// We need 3 coordinates.
			if(tokens.size() < 4){
				continue;
			}
			glm::vec3 pos = glm::vec3(stof(tokens[1],NULL),stof(tokens[2],NULL),stof(tokens[3],NULL));
			positions_temp.push_back(pos);
			
		} else if (tokens[0] == "vn"){ // Vertex normal
			// We need 3 coordinates.
			if(tokens.size() < 4){
				continue;
			}
			glm::vec3 nor = glm::vec3(stof(tokens[1],NULL),stof(tokens[2],NULL),stof(tokens[3],NULL));
			normals_temp.push_back(nor);
			
		} else if (tokens[0] == "vt") { // Vertex UV
			// We need 2 coordinates.
			if(tokens.size() < 3){
				continue;
			}
			glm::vec2 uv = glm::vec2(stof(tokens[1],NULL),stof(tokens[2],NULL));
			texcoords_temp.push_back(uv);
			
		} else if (tokens[0] == "f") { // Face indices.
			// We need 3 elements, each containing at most three indices.
			if(tokens.size() < 4){
				continue;
			}
			faces_temp.push_back(tokens[1]);
			faces_temp.push_back(tokens[2]);
			faces_temp.push_back(tokens[3]);

		} else { // Ignore s, l, g, matl or others
			continue;
		}
	}
	in.close();

	// If no vertices, end.
	if(positions_temp.size() == 0){
			return;
	}

	// Does the mesh have UV or normal coordinates ?
	bool hasUV = texcoords_temp.size()>0;
	bool hasNormals = normals_temp.size()>0;

	// Depending on the chosen extraction mode, we fill the mesh arrays accordingly.
	if (mode == Points){
		// Mode: Points
		// In this mode, we don't care about faces. We simply associate each vertex/normal/uv in the same order.
		
		mesh.positions = positions_temp;
		if(hasNormals){
			mesh.normals = normals_temp;
		}
		if(hasUV){
			mesh.texcoords = texcoords_temp;
		}

	} else if(mode == Expanded){
		// Mode: Expanded
		// In this mode, vertices are all duplicated. Each face has its set of 3 vertices, not shared with any other face.
		
		// For each face, query the needed positions, normals and uvs, and add them to the mesh structure.
		for(int i = 0; i < faces_temp.size(); i++){
			string str = faces_temp[i];
			size_t foundF = str.find_first_of("/");
			size_t foundL = str.find_last_of("/");
			
			// Positions (we are sure they exist).
			long ind1 = stol(str.substr(0,foundF))-1;
			mesh.positions.push_back(positions_temp[ind1]);

			// UVs (second index).
			if(hasUV){
				long ind2 = stol(str.substr(foundF+1,foundL))-1;
				mesh.texcoords.push_back(texcoords_temp[ind2]);
			}

			// Normals (third index, in all cases).
			if(hasNormals){
				long ind3 = stol(str.substr(foundL+1))-1;
				mesh.normals.push_back(normals_temp[ind3]);
			}
			
			//Indices (simply a vector of increasing integers).
			mesh.indices.push_back(i);
		}

	} else if (mode == Indexed){
		// Mode: Indexed
		// In this mode, vertices are only duplicated if they were already used in a previous face with a different set of uv/normal coordinates.
		
		// Keep track of previously encountered (position,uv,normal).
		map<string,long> indices_used;

		//Positions
		long maxInd = 0;
		for(int i = 0; i < faces_temp.size(); i++){
			
			string str = faces_temp[i];

			//Does the association of attributs already exists ?
			if(indices_used.count(str)>0){
				// Just store the index in the indices vector.
				mesh.indices.push_back(indices_used[str]);
				// Go to next face.
				continue;
			}

			// else, query the associated position/uv/normal, store it, update the indices vector and the list of used elements.
			size_t foundF = str.find_first_of("/");
			size_t foundL = str.find_last_of("/");
			
			//Positions (we are sure they exist)
			long ind1 = stol(str.substr(0,foundF))-1;
			mesh.positions.push_back(positions_temp[ind1]);

			//UVs (second index)
			if(hasUV){
				long ind2 = stol(str.substr(foundF+1,foundL))-1;
				mesh.texcoords.push_back(texcoords_temp[ind2]);
			}
			//Normals (third index, in all cases)
			if(hasNormals){
				long ind3 = stol(str.substr(foundL+1))-1;
				mesh.normals.push_back(normals_temp[ind3]);
			}

			mesh.indices.push_back(maxInd);
			indices_used[str] = maxInd;
			maxInd++;
		}
		indices_used.clear();
	}

	positions_temp.clear();
	normals_temp.clear();
	texcoords_temp.clear();
	faces_temp.clear();
	cout << "OBJ loaded. " << mesh.indices.size()/3 << " faces, " << mesh.positions.size() << " vertices, " << mesh.normals.size() << " normals, " << mesh.texcoords.size() << " texcoords." <<  endl;
	return;
}

void centerAndUnitMesh(mesh_t & mesh){
	// Compute the centroid.
	glm::vec3 centroid = glm::vec3(0.0);
	float maxi = mesh.positions[0].x;
	for(int i = 0; i < mesh.positions.size(); i++){
		centroid += mesh.positions[i];
	}
	centroid /= mesh.positions.size();

	for(int i = 0; i < mesh.positions.size(); i++){
		// Translate  the vertex.
		mesh.positions[i] -= centroid;
		// Find the maximal distance from a vertex to the center.
		maxi = abs(mesh.positions[i].x) > maxi ? abs(mesh.positions[i].x) : maxi;
		maxi = abs(mesh.positions[i].y) > maxi ? abs(mesh.positions[i].y) : maxi;
		maxi = abs(mesh.positions[i].z) > maxi ? abs(mesh.positions[i].z) : maxi;
	}
	maxi = maxi == 0.0 ? 1.0 : maxi;
	
	// Scale the mesh.
	for(int i = 0; i < mesh.positions.size(); i++){
		mesh.positions[i] /= maxi;
	}
}