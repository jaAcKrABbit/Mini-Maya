#include "mesh.h"

Mesh::Mesh(OpenGLContext* context):Drawable(context), faces{}, halfEdges{}, vertices{}, binded(false)
{

}

void Mesh::create() {

    std::vector<glm::vec4> pos;
    std::vector<glm::vec4> nor;
    std::vector<glm::vec4> color;
    std::vector<glm::ivec2> jointID;
    std::vector<glm::vec2> weight;
    std::vector<GLuint> idx;
    GLuint faceCount = 0;
    for(auto& face: faces) {
        HalfEdge* e = face->halfEdge;
        int n = pos.size();
        int edgeCount = 0;
        do {
            //normals on a face is all the same
            glm::vec3 faceNormal;
            HalfEdge* p = e;
            glm::vec3 v_0;
            glm::vec3 v_1;
            Vertex* vert = e->vertex;
            //check vert on the same line
            do {
                v_0 = p->next->vertex->position - p->vertex->position;
                v_1 = p->next->next->vertex->position - p->next->vertex->position;
                faceNormal = glm::cross(v_0, v_1);
                p = p->next;
            } while (glm::length(faceNormal) < FLT_EPSILON);
            faceNormal = glm::normalize(faceNormal);
            pos.push_back(glm::vec4(e->vertex->position,1));
            nor.push_back(glm::vec4(faceNormal,0));
            //influenceJoints: pair(j,w)
            if(binded) {
                int j1 = vert->influenceJoints.at(0).first->id;
                int j2 = vert->influenceJoints.at(1).first->id;
                float w1 = vert->influenceJoints.at(0).second;
                float w2 = vert->influenceJoints.at(1).second;
                jointID.push_back(glm::ivec2(j1,j2));
                weight.push_back(glm::vec2(w1,w2));
            }

            color.push_back(glm::vec4(face->color,1));
            e = e->next;
            edgeCount++;
         } while(e != face->halfEdge);
        //add idx
        for(int i = 0; i < edgeCount-2; i++) {
           idx.push_back(n);
           idx.push_back(n + i + 1);
           idx.push_back(n + i + 2);
        }
        faceCount++;
    }
    count = idx.size();


    generateIdx();
    mp_context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufIdx);
    mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(GLuint), idx.data(), GL_STATIC_DRAW);

    generatePos();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, bufPos);
    mp_context->glBufferData(GL_ARRAY_BUFFER, pos.size() * sizeof(glm::vec4), pos.data(), GL_STATIC_DRAW);

    generateNor();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, bufNor);
    mp_context->glBufferData(GL_ARRAY_BUFFER, nor.size() * sizeof(glm::vec4), nor.data(), GL_STATIC_DRAW);

    generateCol();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, bufCol);
    mp_context->glBufferData(GL_ARRAY_BUFFER, color.size() * sizeof(glm::vec4), color.data(), GL_STATIC_DRAW);

    //id and weight
    generateJointID();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, bufJointID);
    mp_context->glBufferData(GL_ARRAY_BUFFER, jointID.size() * sizeof(glm::ivec2), jointID.data(), GL_STATIC_DRAW);

    generateWeight();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, bufWeight);
    mp_context->glBufferData(GL_ARRAY_BUFFER, weight.size() * sizeof(glm::vec2), weight.data(), GL_STATIC_DRAW);

}

GLenum Mesh::drawMode() {
    return GL_TRIANGLES;
}


void Mesh::createCube(){

    const std::vector<glm::vec3> colors{COLOR_GREEN, COLOR_RED, COLOR_BLUE,
                                        COLOR_YELLOW,COLOR_CYAN,COLOR_PURPLE};

    const std::vector<glm::vec3> pos = {   //front
                                        glm::vec3(-0.5, -0.5, 0.5),
                                        glm::vec3(0.5, -0.5, 0.5),
                                        glm::vec3(0.5, 0.5, 0.5),
                                        glm::vec3(-0.5, 0.5, 0.5),
                                        //back
                                        glm::vec3(-0.5, 0.5, -0.5),
                                        glm::vec3(0.5, 0.5, -0.5),
                                        glm::vec3(0.5, -0.5, -0.5),
                                        glm::vec3(-0.5, -0.5, -0.5)};
    // 6 faces
    for(int i = 0; i < 6; i++) {
        uPtr<Face> face = mkU<Face>();
        face->color = colors[i];
        //4 half edges per face
        for(int j = 0; j < 4; j++) {
            uPtr<HalfEdge> halfedge = mkU<HalfEdge>();
            //edge->face
            halfedge->face = face.get();
            halfEdges.push_back(std::move(halfedge));
        }
        //edge->next
        for (int j = 4 * i; j < 4 * i + 4; j++) {
            if(j != 4 * i + 3){
                halfEdges[j]->next = halfEdges[j+1].get();
            }else{
                halfEdges[j]->next = halfEdges[4 * i].get();
            }
        }
        //face points to the first halfedge pushed into the vec
        face->halfEdge = halfEdges[4 * i].get();
        faces.push_back(std::move(face));
    }
    //define verts
    for(int i = 0; i < 8; i++) {
        uPtr<Vertex> vertex = mkU<Vertex>(pos[i][0],pos[i][1], pos[i][2]);
        if(i < 4){
            vertex->halfEdge = halfEdges[i].get();
        } else{
            vertex->halfEdge = halfEdges[i+4].get();
        }
        vertices.push_back(std::move(vertex));
    }
    // edge->vertex
    setVertexForEdge();
    setSymForEdge();
    create();
}

void Mesh::setVertexForEdge() {
    halfEdges[0]->vertex = vertices[0].get();
    halfEdges[1]->vertex = vertices[1].get();
    halfEdges[2]->vertex = vertices[2].get();
    halfEdges[3]->vertex = vertices[3].get();
    halfEdges[4]->vertex = vertices[3].get();
    halfEdges[5]->vertex = vertices[2].get();
    halfEdges[6]->vertex = vertices[5].get();
    halfEdges[7]->vertex = vertices[4].get();
    halfEdges[8]->vertex = vertices[4].get();
    halfEdges[9]->vertex = vertices[5].get();
    halfEdges[10]->vertex = vertices[6].get();
    halfEdges[11]->vertex = vertices[7].get();
    halfEdges[12]->vertex = vertices[7].get();
    halfEdges[13]->vertex = vertices[6].get();
    halfEdges[14]->vertex = vertices[1].get();
    halfEdges[15]->vertex = vertices[0].get();
    halfEdges[16]->vertex = vertices[7].get();
    halfEdges[17]->vertex = vertices[0].get();
    halfEdges[18]->vertex = vertices[3].get();
    halfEdges[19]->vertex = vertices[4].get();
    halfEdges[20]->vertex = vertices[1].get();
    halfEdges[21]->vertex = vertices[6].get();
    halfEdges[22]->vertex = vertices[5].get();
    halfEdges[23]->vertex = vertices[2].get();
}

void Mesh::setSymForEdge() {
    halfEdges[0]->sym = halfEdges[18].get();
    halfEdges[18]->sym = halfEdges[0].get();
    halfEdges[1]->sym = halfEdges[15].get();
    halfEdges[15]->sym = halfEdges[1].get();
    halfEdges[2]->sym = halfEdges[20].get();
    halfEdges[20]->sym = halfEdges[2].get();
    halfEdges[3]->sym = halfEdges[5].get();
    halfEdges[5]->sym = halfEdges[3].get();
    halfEdges[4]->sym = halfEdges[19].get();
    halfEdges[19]->sym = halfEdges[4].get();
    halfEdges[6]->sym = halfEdges[23].get();
    halfEdges[23]->sym = halfEdges[6].get();
    halfEdges[7]->sym = halfEdges[9].get();
    halfEdges[9]->sym = halfEdges[7].get();
    halfEdges[8]->sym = halfEdges[16].get();
    halfEdges[16]->sym = halfEdges[8].get();
    halfEdges[10]->sym = halfEdges[22].get();
    halfEdges[22]->sym = halfEdges[10].get();
    halfEdges[11]->sym = halfEdges[13].get();
    halfEdges[13]->sym = halfEdges[11].get();
    halfEdges[14]->sym = halfEdges[21].get();
    halfEdges[21]->sym = halfEdges[14].get();
    halfEdges[17]->sym = halfEdges[12].get();
    halfEdges[12]->sym = halfEdges[17].get();
}

void Mesh::splitEdge(HalfEdge* e1) {
    HalfEdge* e2 = e1->sym;
    Vertex* v1 = e1->vertex;
    Vertex* v2 = e2->vertex;
    glm::vec3 v3_pos = (v1->position + v2->position) / 2.0f;
    uPtr<Vertex> v3 = mkU<Vertex>(v3_pos.x, v3_pos.y, v3_pos.z);
    uPtr<HalfEdge> e1b = mkU<HalfEdge>();
    uPtr<HalfEdge> e2b = mkU<HalfEdge>();

    e1b->vertex = v1;
    e2b->vertex = v2;
    e1b->face = e1->face;
    e2b->face = e2->face;
    e1->sym = e2b.get();
    e2b->sym = e1;
    e1b->sym = e2;
    e2->sym = e1b.get();
    e1b->next = e1->next;
    e1->next = e1b.get();
    e2b->next = e2->next;
    e2->next = e2b.get();
    e1->vertex = v3.get();
    e2->vertex = v3.get();

    halfEdges.push_back(std::move(e1b));
    halfEdges.push_back(std::move(e2b));
    vertices.push_back(std::move(v3));
}

void Mesh::triangulate(Face* f1) {

    HalfEdge* e0 = f1->halfEdge;
    int edgeCount = 0;
    do {
        edgeCount++;
        e0 = e0->next;
    }while(e0 != f1->halfEdge);

    if(edgeCount > 3) {
        for(int i = 0; i< edgeCount - 3; i++){
            uPtr<HalfEdge> ea = mkU<HalfEdge>();
            uPtr<HalfEdge> eb = mkU<HalfEdge>();
            ea->vertex = e0->vertex;
            eb->vertex = e0->next->next->vertex;
            ea->sym = eb.get();
            eb->sym = ea.get();
            uPtr<Face> f2 = mkU<Face>();
            ea->face = f2.get();
            e0->next->face = f2.get();
            e0->next->next->face = f2.get();
            eb->face = f1;
            f2->halfEdge = ea.get();
            eb->next = e0->next->next->next;
            e0->next->next->next = ea.get();
            ea->next = e0->next;
            e0->next = eb.get();
            f2->color = f1->color;
            halfEdges.push_back(std::move(ea));
            halfEdges.push_back(std::move(eb));
            faces.push_back(std::move(f2));
        }
    }
    //check vert on same line
}

void Mesh::subdivide() {
    std::map<Face*, Vertex*> centroids_map;
    std::map<HalfEdge*, Vertex*> midpoints_map;
    std::vector<Vertex*> oldVerts;
    std::vector<Face*> oldFaces;
    std::vector<HalfEdge*> oldEdges;

    for(auto& vert: vertices) {
        oldVerts.push_back(vert.get());
    }

    for(auto& face: faces) {
        oldFaces.push_back(face.get());
    }

    for(auto& edge: halfEdges) {
        oldEdges.push_back(edge.get());
    }

    //get centroids
    for(auto& face: faces) {
        uPtr<Vertex> centroid = mkU<Vertex>();
        HalfEdge* e = face->halfEdge;
        int sides = 0;
        do {
            centroid->position += e->vertex->position;
            e = e->next;
            sides++;
        }while(e != face->halfEdge);
        centroid->position /= sides;
        centroids_map.insert(std::pair<Face*, Vertex*>(face.get(), centroid.get()));
        vertices.push_back(std::move(centroid));
    }
    //get midpoints
    //only need one halfedge per edge
    std::vector<HalfEdge*> edges;
    for(auto& edge: oldEdges) {
        if(std::find(edges.begin(), edges.end(), edge->sym) == edges.end()) {
            edges.push_back(edge);
        }
    }

    //temp midpoints vector
    std::vector<Vertex*> midpoints;
    for(auto& edge: edges) {
        uPtr<Vertex> mp = mkU<Vertex>();
        Vertex* v1 = edge->vertex;
        Vertex* v2 = edge->sym->vertex;
        Vertex* f1 = centroids_map.find(edge->face)->second;
        Vertex* f2 = centroids_map.find(edge->sym->face)->second;

        if(edge->sym->face) {
            mp->position = (v1->position + v2->position + f1->position + f2->position) / 4.f;
        }else {
            mp->position = (v1->position + v2->position + f1->position) / 3.f;
        }
        mp->halfEdge = edge;
        midpoints_map.insert(std::pair<HalfEdge*, Vertex*>(edge, mp.get()));
        vertices.push_back(std::move(mp));
    } 
    // smooth
    for(auto& v: oldVerts) {
        float n = 0.f;
        glm::vec3 sum_e;
        glm::vec3 sum_f;
        HalfEdge* e = v->halfEdge;// had bug
        do {
            auto iter = midpoints_map.find(e);
            if(iter != midpoints_map.end()) {
                sum_e += midpoints_map.find(e)->second->position;
            }else{
                sum_e += midpoints_map.find(e->sym)->second->position;
            }
            sum_f += centroids_map.find(e->face)->second->position;
            n++;
            e = e->next->sym;
        }while(e != v->halfEdge);
        v->position = (n - 2.f) * v->position / n + sum_e / (n * n) + sum_f / (n * n);
    }
    //splitedges
    for(auto& e1: edges) {
        HalfEdge* e2 = e1->sym;
        Vertex* v1 = e1->vertex;
        Vertex* v2 = e2->vertex;

        uPtr<HalfEdge> e1b = mkU<HalfEdge>();
        uPtr<HalfEdge> e2b = mkU<HalfEdge>();

        e1b->vertex = v1;
        e2b->vertex = v2;
        e1b->face = e1->face;
        e2b->face = e2->face;
        e1->sym = e2b.get();
        e2b->sym = e1;
        e1b->sym = e2;
        e2->sym = e1b.get();
        e1b->next = e1->next;
        e1->next = e1b.get();
        e2b->next = e2->next;
        e2->next = e2b.get();
        e1->vertex = midpoints_map.find(e1)->second;
        e2->vertex = midpoints_map.find(e1)->second;

        halfEdges.push_back(std::move(e1b));
        halfEdges.push_back(std::move(e2b));
    }
    //quadrangulate
    for(auto& face: oldFaces) {
        Vertex* centroid = centroids_map.find(face)->second;
        HalfEdge* e = face->halfEdge;
        std::vector<HalfEdge*> tempEdges;
        do{
            tempEdges.push_back(e);
            e = e->next->next;
        }while(e != face->halfEdge);
        quadrangulate(face, centroid, tempEdges);
    }
}

void Mesh::quadrangulate(Face* face, Vertex* centroid, std::vector<HalfEdge*> &edges) {

    //number of vertices on the original face
    int n = edges.size();
    std::vector<Face*> newFaces;
    std::vector<HalfEdge*> prevEdges;

    //original face
    newFaces.push_back(face);
    //create n-1 more faces
    for(int i = 0; i < n-1; i++) {
        uPtr<Face> newFace = mkU<Face>();
        newFace->color = face->color;
        newFaces.push_back(newFace.get());
        faces.push_back(std::move(newFace));
    }
    //get edge.prev
    for(auto& edge: edges) {
        prevEdges.push_back(edge->getPrev());
    }
    //edge index for edges
    int i = 0;
    //setup mesh components for faces
    for(auto& face: newFaces) {
        HalfEdge* edge = edges[i];
        HalfEdge* prev = prevEdges[i];
        face->halfEdge = edge;
        uPtr<HalfEdge> e1 = mkU<HalfEdge>();
        uPtr<HalfEdge> e2 = mkU<HalfEdge>();

        e1->vertex = centroid;
        centroid->halfEdge = e1.get();
        e2->vertex = prev->sym->vertex;

        prev->vertex->halfEdge = prev;

        //edge.pre
        e2->next = prev;
        e1->next = e2.get();
        edge->next = e1.get();

        edge->face = face;
        e1->face = face;
        e2->face = face;
        prev->face = face;
        halfEdges.push_back(std::move(e1));
        halfEdges.push_back(std::move(e2));
        i++;
    }
    // set sym
    for(int i = 0; i < n; i++) {
        if(i == 0) {
            edges[i]->next->sym = edges[i+1]->next->next;
            edges[i]->next->next->sym = edges[n-1]->next;
        }else if (i == n - 1) {
            edges[i]->next->sym = edges[0]->next->next;
            edges[i]->next->next->sym = edges[i-1]->next;
        }else{
            //e1 = next.e2
            edges[i]->next->sym = edges[i+1]->next->next;
            //e2 = prev.e1
            edges[i]->next->next->sym = edges[i-1]->next;
        }
    }

}

glm::vec3 generateRandomColor() {
    glm::vec3 color = glm::vec3(1.f);
    color[0] = 0.f;
    for(int i = 1; i < 3; i++) {
        color[i] = (float) rand() / (RAND_MAX);
    }
    return color;
}

void Mesh::createObj(const QString filePath) {
    QFile objFile(filePath);
    objFile.open(QIODevice::ReadOnly);
    if (!objFile.isOpen()){
        qWarning("Could not open the OBJ file.");
        return;
    }
    clearMeshData();
    QTextStream stream(&objFile);

    std::map<std::pair<int,int>, HalfEdge*> symMap;

    for (QString line = stream.readLine(); !line.isNull(); line = stream.readLine()) {
        QStringList data = line.split(" ");
        QStringList vertsData;
        QString type = data[0];
        if(type == "v") {
            uPtr<Vertex> v = mkU<Vertex>(data[1].toFloat(), data[2].toFloat(), data[3].toFloat());
            vertices.push_back(std::move(v));
        }else if(type == "f") {
            uPtr<Face> f = mkU<Face>();
            f->color = generateRandomColor();
            int vertCount = data.size() - 1;
            for(int i = 0; i < vertCount; i++) {
                uPtr<HalfEdge> he = mkU<HalfEdge>();
                vertsData = data[i+1].split("/");
                int vertID = vertsData[0].toInt() - 1;

                he->vertex = vertices[vertID].get();
                vertices[vertID]->halfEdge = he.get();
                he->face = f.get();
                f->halfEdge = he.get();
                halfEdges.push_back(std::move(he));
            }
            faces.push_back(std::move(f));
            //link half edges, last vertCount halfedges are what just pushed
            for(int i = 0; i < vertCount; i++) {
                if(i == vertCount -1) {
                    halfEdges.back()->next = halfEdges[halfEdges.size() - vertCount].get();
                }else{
                    halfEdges[halfEdges.size() - vertCount + i]->next = halfEdges[halfEdges.size() - vertCount + i + 1].get();
                }
                vertsData = data[i+1].split("/");
                HalfEdge* current = halfEdges[halfEdges.size() - vertCount + i].get();
                int currentVert = vertsData[0].toInt();
                int prevVert = i == 0? data[vertCount].split("/")[0].toInt():data[i].split("/")[0].toInt();
                std::pair<int,int> key(currentVert, prevVert);
                auto iter = symMap.find(key);
                if(iter != symMap.end()) {
                    current->sym = iter->second;
                    iter->second->sym = current;
                }else{
                    symMap.insert(std::make_pair(std::make_pair(prevVert, currentVert), current));
                }
            }
        }
    }
    objFile.close();
}

void Mesh::extrude(Face *face) {
    const float distance = 1;
    HalfEdge* e = face->halfEdge;
    glm::vec3 faceNormal;
    glm::vec3 v_0;
    glm::vec3 v_1;
    v_0 = e->next->vertex->position - e->vertex->position;
    v_1 = e->getPrev()->vertex->position - e->vertex->position;
    faceNormal = glm::cross(v_0, v_1);
    faceNormal = glm::normalize(faceNormal);
    do {
        uPtr<Vertex> v = mkU<Vertex>();
        v->position = e->vertex->position + faceNormal * distance;
        v->halfEdge = e;
        e->vertex = v.get();
        vertices.push_back(std::move(v));
        e = e->next;
     } while(e != face->halfEdge);

    do {
        HalfEdge* he2 = e->sym;

        uPtr<HalfEdge> he1b = mkU<HalfEdge>();
        uPtr<HalfEdge> he2b = mkU<HalfEdge>();

        Vertex *v1 = he2->getPrev()->vertex;
        Vertex *v2 = he2->vertex;
        Vertex *v3 = e->vertex;
        Vertex *v4 = e->getPrev()->vertex;

        e->sym = he1b.get();
        he2->sym = he2b.get();
        he1b->sym = e;
        he2b->sym = he2;

        he1b->vertex = v4;
        he2b->vertex = v1;

        uPtr<HalfEdge> he3 = mkU<HalfEdge>();
        uPtr<HalfEdge> he4 = mkU<HalfEdge>();
        uPtr<Face> f = mkU<Face>();

        he1b->face = f.get();
        he2b->face = f.get();
        he3->face = f.get();
        he4->face = f.get();

        he3->vertex = v3;
        he4->vertex = v2;

        he1b->next = he4.get();
        he4->next = he2b.get();
        he2b->next = he3.get();
        he3->next = he1b.get();

        f->halfEdge = he2b.get();
        f->color = e->face->color;

        faces.push_back(std::move(f));
        halfEdges.push_back(std::move(he1b));
        halfEdges.push_back(std::move(he2b));
        halfEdges.push_back(std::move(he3));
        halfEdges.push_back(std::move(he4));

        e = e->next;
    }while(e != face->halfEdge);

    do {
        e->sym->getPrev()->sym = e->next->sym->next;
        e->next->sym->next->sym = e->sym->getPrev();
        e = e->next;
    } while (e != face->halfEdge);
}

void Mesh::bindSkeleton() {
    for(auto& vert: vertices) {
        std::vector<Joint*> closestJoints;
        std::vector<float> closestDistances;
        //ordered map to sort distance in ascending order
        std::map<float,Joint*> distances;
        float distance;
        //closest distances
        glm::vec4 vertPos = glm::vec4(vert->position,1);
        for(auto& joint: Joint::skeleton) {
            glm::vec4 jointPos = joint->getOverallTransformation() * glm::vec4(0, 0, 0, 1);
            distance = glm::length(jointPos - vertPos);
            distances.insert(std::pair<float,Joint*>(distance, joint.get()));
        }

        for(auto& pair: distances){
            if(closestJoints.size() == 2){
                break;
            }
            closestDistances.push_back(pair.first);
            closestJoints.push_back(pair.second);
        }

        Joint* j1 = closestJoints[0];
        Joint* j2 = closestJoints[1];
        float d1 = closestDistances[0];
        float d2 = closestDistances[1];

        float distanceSum = d1 + d2;
        float w1 = 1 - d1/distanceSum;
        float w2 = 1 - d2/distanceSum;
        vert->influenceJoints.push_back(std::make_pair(j1, w1));
        vert->influenceJoints.push_back(std::make_pair(j2, w2));
    }
    binded = true;
}

void Mesh::clearMeshData() {
    faces.clear();
    halfEdges.clear();
    vertices.clear();
    Face::last = 0;
    Vertex::last = 0;
    HalfEdge::last = 0;
}


