#include "objects.hh"
#include <string>

using namespace objects;

int Object::obj_count = 0;

Object::Object(std::string name, ignition::math::Pose3d pose):
name(name + "_" + std::to_string(obj_count)), pose(pose){
    this->obj_count++;
};


boost::shared_ptr<sdf::SDF> Object::GetSDF(){
    boost::shared_ptr<sdf::SDF> sdf = boost::make_shared<sdf::SDF>();
    return sdf;
}

void Object::AddToWorld(gazebo::physics::WorldPtr world){
    auto sdf = this->GetSDF();
    world->InsertModelSDF(*sdf);
}

Box::Box(ignition::math::Box box, std::string name):
Object(name, ignition::math::Pose3d(box.Min(), ignition::math::Quaterniond(0,0,0,0))){
    this->box = box;
}

boost::shared_ptr<sdf::SDF> Box::GetSDF(){

    auto center = (this->box.Min() + this->box.Max())/2;
    auto width = this->box.Max().X() - this->box.Min().X();
    auto depth = this->box.Max().Y() - this->box.Min().Y();
    auto height = this->box.Max().Z() - this->box.Min().Z();

    boost::shared_ptr<sdf::SDF> boxSDF = boost::make_shared<sdf::SDF>();
    boxSDF->SetFromString(
       "<sdf version ='1.6'>\
          <model name ='box'>\
            <pose>0 0 0 0 0 0</pose>\
            <link name ='link'>\
              <collision name ='collision'>\
                <geometry>\
                    <box>\
                        <size>0 0 0</size>\
                    </box>\
                </geometry>\
              </collision>\
              <visual name ='visual'>\
                <geometry>\
                    <box>\
                        <size>0 0 0</size>\
                    </box>\
                </geometry>\
              </visual>\
            </link>\
          </model>\
        </sdf>");
    
    boxSDF->Root()->GetElement("model")->GetAttribute("name")->SetFromString(this->name);
    boxSDF->Root()->GetElement("model")->GetElement("pose")->Set(ignition::math::Pose3d(center, ignition::math::Quaterniond(0,0,0,0)));
    boxSDF->Root()->GetElement("model")->GetElement("link")->GetElement("collision")->GetElement("geometry")->GetElement("box")->GetElement("size")->Set(ignition::math::Vector3d(width,depth,height));
    boxSDF->Root()->GetElement("model")->GetElement("link")->GetElement("visual")->GetElement("geometry")->GetElement("box")->GetElement("size")->Set(ignition::math::Vector3d(width,depth,height));
    

    return boxSDF;
}


Boxes::Boxes(std::string name): Object(name, ignition::math::Pose3d(0,0,0,0,0,0)){

}

void Boxes::AddBox(ignition::math::Box box){
    this->boxes.push_back(box);
}

boost::shared_ptr<sdf::SDF> Boxes::GetSDF(){
    int count = 0;

    boost::shared_ptr<sdf::SDF> sdf = boost::make_shared<sdf::SDF>();
    sdf->SetFromString(
       "<sdf version ='1.6'>\
          <model name ='box'>\
            <pose>0 0 0 0 0 0</pose>\
          </model>\
        </sdf>");

    auto model = sdf->Root()->GetElement("model");
    model->GetElement("static")->Set(true);
    

    for (auto box: this->boxes){

        auto center = (box.Min() + box.Max())/2;
        auto width = box.Max().X() - box.Min().X();
        auto depth = box.Max().Y() - box.Min().Y();
        auto height = box.Max().Z() - box.Min().Z();

        auto link = model->AddElement("link");
        link->GetAttribute("name")->SetFromString("l" + std::to_string(count));
        link->GetElement("pose")->Set(ignition::math::Pose3d(center, ignition::math::Quaterniond(0,0,0,0)));
        link->GetElement("collision")->GetElement("geometry")->GetElement("box")->GetElement("size")->Set(ignition::math::Vector3d(width,depth,height));
        // link->GetElement("collision")->GetElement("surface")->GetElement("friction")->GetElement("ode")->GetElement("mu")->Set(1);
        // link->GetElement("collision")->GetElement("surface")->GetElement("friction")->GetElement("ode")->GetElement("mu2")->Set(1);
        // link->GetElement("self_collide")->Set(false);
        link->GetElement("visual")->GetElement("geometry")->GetElement("box")->GetElement("size")->Set(ignition::math::Vector3d(width,depth,height));
        count++;
    }

    return sdf;
}