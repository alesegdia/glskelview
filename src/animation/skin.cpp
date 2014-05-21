#include "skin.h"

Skin::Skin (const char* path)
{
	vertices=NULL;
	if(path) Parse(path);
}

Skin::~Skin ()
{

	Reset();
}

void Skin::Reset ()
{
	if(vertices) delete [] vertices;
	if(vertexbuffer) delete [] vertexbuffer;
	if(normalbuffer) delete [] normalbuffer;
	if(trisbuffer) delete [] trisbuffer;
}

void Skin::SetSkel (Skeleton* skel)
{
	this->skel = skel;
}

void Skin::Parse (const char* path)
{
	Reset();

	Tokenizer tok;
	tok.Open(path);

	/* PARSE VERTICES ******************/
	printf("Parsing vertices... ");
	tok.FindToken("positions");
	vertexNum=tok.GetInt();
	vertices = new vertex_t[vertexNum];
	vertexbuffer = new GLfloat[vertexNum*3];
	printf("n vertices: %d\n",vertexNum);
	tok.FindToken("{");
	for(int i=0;i<vertexNum;i++) {
		float x,y,z;
		x=tok.GetFloat();
		y=tok.GetFloat();
		z=tok.GetFloat();
		vertices[i].Position(Vector3(x,y,z));
		vertexbuffer[(i*3)+0] = vertices[i].Position().x;
		vertexbuffer[(i*3)+1] = vertices[i].Position().y;
		vertexbuffer[(i*3)+2] = vertices[i].Position().z;
	}
	tok.FindToken("}");
	printf(" PASS!\n");

	/* PARSE NORMALS *******************/
	printf("Parsing normals... ");
	tok.FindToken("normals");
	normalsNum = tok.GetInt();
	normalbuffer = new GLfloat[normalsNum*3];
	tok.FindToken("{");
	for(int i=0;i<normalsNum;i++) {
		float x,y,z;
		x=tok.GetFloat();
		y=tok.GetFloat();
		z=tok.GetFloat();
		vertices[i].Normal(Vector3(x,y,z));
		normalbuffer[(i*3)+0] = vertices[i].Normal().x;
		normalbuffer[(i*3)+1] = vertices[i].Normal().y;
		normalbuffer[(i*3)+2] = vertices[i].Normal().z;
	}
	tok.FindToken("}");
	printf(" PASS!\n");

	/* PARSE SKINWEIGHTS ***************/
	printf("Parsing skin weights... ");
	tok.FindToken("skinweights");
	int weightsNum = tok.GetInt();
	tok.FindToken("{");
	for(int i=0;i<weightsNum;i++) {
		int bonesNum = tok.GetInt();
		for(int j=0;j<bonesNum;j++) {
			int bone=tok.GetInt();
			float weight=tok.GetFloat();
			vertices[i].AddWeight(bone, weight);
		}
	}
	tok.FindToken("}");
	printf(" PASS!\n");

	/* PARSE TRIANGLES ****************/
	printf("Parsing triangles... ");
	tok.FindToken("triangles");
	trisNum = tok.GetInt();
	tris = new triangle_t[trisNum];
	trisbuffer = new GLuint[trisNum*3];
	tok.FindToken("{");
	for(int i=0;i<trisNum;i++) {
		for(int j=0;j<3;j++) {
			tris[i][j]=tok.GetInt();
			trisbuffer[(i*3)+j] = tris[i][j];
		}
	}
	tok.FindToken("}");
	printf(" PASS!\n");

	/* PARSE BINDINGS *****************/
	printf("Parsing bindings...");
	tok.FindToken("bindings");
	int bindingsNum = tok.GetInt();
	bindings = new Matrix34[bindingsNum];
	tok.FindToken("{");
	for(int i=0;i<bindingsNum;i++) {
		tok.FindToken("matrix");
		tok.FindToken("{");
		for(int j=0;j<4;j++) {
			float x,y,z;
			x=tok.GetFloat();
			y=tok.GetFloat();
			z=tok.GetFloat();
			bindings[i][j].Set(x,y,z);
		}
		bindings[i].Inverse();
		tok.FindToken("}");
	}
	tok.FindToken("}");
	printf(" PASS!\n");

	tok.Close();
}

void Skin::Update()
{
	for(int i=0;i<vertexNum;i++) {
		vertex_t* v = &(vertices[i]);
		Vector3 sum,normalsum;
		for(int j=0;j<v->NumWeights();j++) {
			Vector3 tmp(v->Position());
			Vector3 normaltmp(v->Normal());
			int index = v->BoneID(j);
			float weight = v->Weight(j);
			Matrix34 mat;

			bindings[index].Transform(tmp,tmp);
			bindings[index].Transform(normaltmp,normaltmp);
			skel->GetWorldMatrix(index).Transform(tmp,tmp);
			skel->GetWorldMatrix(index).Transform(normaltmp,normaltmp);

			tmp.Scale(weight);
			normaltmp.Scale(weight);
			sum.Add(tmp);
			normalsum.Add(normaltmp);
		}
		vertexbuffer[(i*3)+0]=sum.x;
		vertexbuffer[(i*3)+1]=sum.y;
		vertexbuffer[(i*3)+2]=sum.z;
		normalsum.Normalize();
		normalbuffer[(i*3)+0]=normalsum.x;
		normalbuffer[(i*3)+1]=normalsum.y;
		normalbuffer[(i*3)+2]=normalsum.z;
	}
}

void Skin::Draw (Matrix34& mat)
{
	glPushMatrix();
	glLoadIdentity();

	glEnableClientState( GL_NORMAL_ARRAY );
	glEnableClientState( GL_VERTEX_ARRAY );
	glVertexPointer(3,GL_FLOAT,0,vertexbuffer);
	glNormalPointer(GL_FLOAT,0,normalbuffer);
	glDrawElements(GL_TRIANGLES,trisNum*3,GL_UNSIGNED_INT,trisbuffer);
	glDisableClientState( GL_NORMAL_ARRAY );
	glDisableClientState( GL_VERTEX_ARRAY );
	glPopMatrix();
}
