#include "skeleton.h"

// JOINT CLASS ****************************************

Skeleton::Joint::Joint ()
{
	Color.Set(0.f,0.f,1.f);
	selected = false;
	hit = false;
}

Skeleton::Joint::~Joint ()
{
	for(auto it : Children)
	{
		delete it;
	}
}

int Skeleton::Joint::Index ()
{
	return index;
}


bool Skeleton::Joint::Parse (Tokenizer& tok, char& next, Joint* list[])
{
	this->index = next;
	list[index]=this;
	char nameBuffer[256];
	tok.GetToken(nameBuffer);
	Tag.assign(nameBuffer, strlen(nameBuffer));

	tok.FindToken("{");

	char buffer[256];
	bool poseSet, box;
	box = poseSet = false;

	while(true)
	{
		bool limits[3] = {false};
		tok.GetToken(buffer);

		if(strcmp(buffer, "offset")==0)
			Parse3DVector(Offset, tok);
		else if(strcmp(buffer, "boxmin")==0)
		{
			box = true;
			Parse3DVector(BoxMin, tok);
		}
		else if(strcmp(buffer, "boxmax")==0)
		{
			box = true;
			Parse3DVector(BoxMax, tok);
		}
		else if(strcmp(buffer, "pose")==0)
		{
			poseSet = true;
			Parse3DVector(Pose, tok);
		}
		else if(strcmp(buffer, "rotxlimit")==0)
		{
			limits[0]=true;
			Parse2DVector(RotXLimit, tok);
		}
		else if(strcmp(buffer, "rotylimit")==0)
		{
			limits[1]=true;
			Parse2DVector(RotYLimit, tok);
		}
		else if(strcmp(buffer, "rotzlimit")==0)
		{
			limits[2]=true;
			Parse2DVector(RotZLimit, tok);
		}
		else if(buffer[0] == '}')
		{
			if(!limits[0]) {
				RotXLimit.x = -CMAX_ROT;
				RotXLimit.y = CMAX_ROT;
			}
			if(!limits[1]) {
				RotYLimit.x = -CMAX_ROT;
				RotYLimit.y = CMAX_ROT;
			}
			if(!limits[2]) {
				RotZLimit.x = -CMAX_ROT;
				RotZLimit.y = CMAX_ROT;
			}
			if(!box) {
				float size = 0.05f;
				BoxMin.Set(-size,-size,-size);
				BoxMax.Set(size,size,size);
			}
			if(!poseSet) {
				Pose.Set(0.f,0.f,0.f);
			}
			return true;
		}
		else if(strcmp(buffer, "balljoint")==0)
		{
			Joint* joint = new Joint;
			next++;
			joint->Parse(tok,next,list);
			AddChild(joint);
		}
		else
		{
			printf("Error! Unrecognized token: %s.\n", buffer);
			return false;
		}
	}
}

void Skeleton::Joint::ApplyPoseConstraints ()
{
	if (Pose.x < RotXLimit.x) Pose.x = RotXLimit.x;
	else if (Pose.x > RotXLimit.y) Pose.x = RotXLimit.y;

	if (Pose.y < RotYLimit.x) Pose.y = RotYLimit.x;
	else if (Pose.y > RotYLimit.y) Pose.y = RotYLimit.y;

	if (Pose.z < RotZLimit.x) Pose.z = RotZLimit.x;
	else if (Pose.z > RotZLimit.y) Pose.z = RotZLimit.y;
}

void Skeleton::Joint::Parse3DVector (Vector3& vec, Tokenizer& tok)
{
	vec.x = tok.GetFloat();
	vec.y = tok.GetFloat();
	vec.z = tok.GetFloat();
}

void Skeleton::Joint::Parse2DVector (Vector3& vec, Tokenizer& tok)
{
	vec.x = tok.GetFloat();
	vec.y = tok.GetFloat();
}

void Skeleton::Joint::AddChild (Joint* child)
{
	Children.push_back(child);
}

void Skeleton::Joint::Update (Matrix34& ParentW)
{
	hit = false;
	// ajustamos límites
	ApplyPoseConstraints();

	// creamos matrices de rotación y traslación
	Matrix34 off, rot;
	off.MakeTranslate(Offset);
	rot.FromEulers(Pose.x, Pose.y, Pose.z, Matrix34::EULER_XYZ);

	Local.Identity();
	// Local = Rotation * Offset
	off.Dot(rot, off);
	Local.Dot(off, Local);

	// World = Local * ParentW
	World.Dot(Local, ParentW);

	for(auto it : Children)
		it->Update(World);

	HoldColor.x=1.f;
	HoldColor.y=0.f;
	HoldColor.z=0.f;
}

void Skeleton::Update (Matrix34& transform)
{
	Root->Update(transform);
}

float Skeleton::Joint::RayTest (const Ray& ray)
{
	float dist;
	hit=false;
	if( ! RayBoxIntersection(
				ray,
				Box( this->BoxMin, this->BoxMax ),
				this->World, dist))
	{
		return std::numeric_limits<float>::max();
	}
	else
	{
		return dist;
	}

}

void Skeleton::Joint::Draw ()
{
	if(selected) Color.Set(0.f,1.f,0.f);
	else Color.Set(0.f,0.f,1.f);
	glPushMatrix();
	glLoadMatrixf(World);
	if(hit) glColor3f(1.f,0.f,0.f);
	else glColor3f(Color.x,Color.y,Color.z);
	drawBox(BoxMin.x, BoxMin.y, BoxMin.z, BoxMax.x, BoxMax.y, BoxMax.z );
	drawAxis(0.10f);
	glPopMatrix();

	for(auto it : Children)
	{
		it->Draw();
	}
}


// SKELETON CLASS *****************************************

Skeleton::Skeleton (const char* path)
{
	Root = NULL;
	Reset();
	if(path) Parse(path);
}

Skeleton::~Skeleton ()
{
	delete Root;
}

const Matrix34& Skeleton::GetWorldMatrix (int index)
{
	return lista[index]->GetWorldMatrix();
}

void Skeleton::UpdateJointPose (int index, float x, float y, float z)
{
	lista[index]->Pose.Set(x,y,z);
}

void Skeleton::Parse(const char* path)
{
	Tokenizer tok;
	tok.Open(path);
	tok.FindToken("balljoint");

	Root = new Joint;
	char i=0;
	Root->Parse(tok,i,lista);
	tok.Close();
}

void Skeleton::Reset()
{
	if (Root != NULL) delete Root;
	Root = NULL;
}

Skeleton::Joint* Skeleton::RayTest (const Ray& ray)
{
	Joint* it = Root;
	Joint* min=NULL;
	float dmin=std::numeric_limits<float>::max();

	bool done = false;
	std::stack<Joint*> s;
	s.push(Root);
	while(!s.empty())
	{
		it = s.top(); s.pop();
		float dact = it->RayTest(ray);
		if(dact < dmin) {min = it; dmin=dact;}
		for(int i=0; i<it->Children.size(); i++)
		{ s.push(it->Children[i]); }
	}
	if(min!=NULL) min->hit = true;
	return min;
}

void Skeleton::Draw ()
{
	glDisable(GL_LIGHTING);
	glPushAttrib( GL_POLYGON_BIT );
	glPolygonMode(GL_FRONT, GL_LINE);
	Root->Draw();
	glLoadIdentity();
	glPopAttrib();
	glEnable(GL_LIGHTING);
}
