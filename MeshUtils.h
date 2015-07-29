


void makeTriFace(Vertex * va, Vertex* vb, Vertex * vc, 
                vector<Face*> &faceVect, 
                vector<Halfedge*> &edgeVect){
    Face * nextFace = new Face;
    Halfedge *e1, *e2, *e3;

    e1 = new Halfedge;
    e2 = new Halfedge;
    e3 = new Halfedge;

    nextFace -> addVertex(va);
    nextFace -> addVertex(vb);
    nextFace -> addVertex(vc);

    e1->start = va;
    e2->start = vb;
    e3->start = vc;
    e1->end = vb;
    e2->end = vc;
    e3->end = va;

    va->oneOutEdge = e1;
    vb->oneOutEdge = e2;
    vc->oneOutEdge = e3;

    e1->next = e2;
    e2->next = e3;
    e3->next = e1;

    e2->previous = e1;
    e3->previous = e2;
    e1->previous = e3;
    
    e1->heFace = nextFace;
    e2->heFace = nextFace;
    e3->heFace = nextFace;

    nextFace->oneSideEdge = e3;

    faceVect.push_back(nextFace);
    edgeVect.push_back(e1);
    edgeVect.push_back(e2);
    edgeVect.push_back(e3);
}

void makeRectFace(Vertex * va, Vertex* vb, Vertex * vc, Vertex * vd, 
                vector<Face*> &faceVect, 
                vector<Halfedge*> &edgeVect){
    Face * nextFace = new Face;
    Halfedge *e1, *e2, *e3, *e4;

    e1 = new Halfedge;
    e2 = new Halfedge;
    e3 = new Halfedge;
    e4 = new Halfedge;

    nextFace -> addVertex(va);
    nextFace -> addVertex(vb);
    nextFace -> addVertex(vc);
    nextFace -> addVertex(vd);

    e1->start = va;
    e2->start = vb;
    e3->start = vc;
    e4->start = vd;
    e1->end = vb;
    e2->end = vc;
    e3->end = vd;
    e4->end = va;

    va->oneOutEdge = e1;
    vb->oneOutEdge = e2;
    vc->oneOutEdge = e3;
    vd->oneOutEdge = e4;

    e1->next = e2;
    e2->next = e3;
    e3->next = e4;
    e4->next = e1;

    e2->previous = e1;
    e3->previous = e2;
    e4->previous = e3;
    e1->previous = e4;

    e1->heFace = nextFace;
    e2->heFace = nextFace;
    e3->heFace = nextFace;
    e4->heFace = nextFace;

    nextFace->oneSideEdge = e4;

    faceVect.push_back(nextFace);
    edgeVect.push_back(e1);
    edgeVect.push_back(e2);
    edgeVect.push_back(e3);
    edgeVect.push_back(e4);
}

void makePolygonFace(vector<Vertex*> vertices, 
                vector<Face*> &faceVect, 
                vector<Halfedge*> &edgeVect){
    Face * nextFace = new Face;
    vector<Halfedge*> edges;
    vector<Vertex*>::iterator vIt;
    Vertex * currVert;
    Halfedge * currEdge;
    for(vIt = vertices.begin(); vIt < vertices.end(); vIt++) {
        currVert = *vIt;
        Halfedge * tempEdge = new Halfedge;
        tempEdge -> start = *vIt;
        if(vIt == (vertices.end() - 1)) {
            tempEdge -> end = *(vertices.begin());
        } else {
            tempEdge -> end = *(vIt + 1);
        }
        edges.push_back(tempEdge);
        currVert -> oneOutEdge = tempEdge;
        nextFace -> addVertex(*vIt);
    }
    
    vector<Halfedge*>::iterator eIt;
    for(eIt = edges.begin(); eIt < edges.end(); eIt++) {
        currEdge = *eIt;
        if(eIt == (edges.end() - 1)) {
            currEdge -> next = *(edges.begin());
            Halfedge * temp = *(edges.begin());
            temp -> previous = currEdge;
        } else {
            currEdge -> next = *(eIt + 1);
            Halfedge * temp = *(eIt + 1);
            temp -> previous = currEdge;
        }
        currEdge -> heFace = nextFace;
        edgeVect.push_back(*eIt);
    }
    nextFace -> oneSideEdge = *(edges.begin());
    faceVect.push_back(nextFace);
}

void makeBoundaries(vector<vector<Vertex*> > &boundaries, vector<Halfedge*> &edgeVect){
    vector<vector<Vertex*> >::iterator allBoundsIt;
    vector<Vertex*> oneBoundary;
    vector<Halfedge*> boundaryEdges;
    vector<Halfedge*>::iterator eIt;
    for(allBoundsIt = boundaries.begin(); allBoundsIt < boundaries.end(); allBoundsIt++) {
        oneBoundary = *allBoundsIt;
        vector<Vertex*>::iterator boundIt;
        for(boundIt = oneBoundary.begin(); boundIt < oneBoundary.end(); boundIt ++) {
            for(eIt = edgeVect.begin(); eIt < edgeVect.end(); eIt++) {
                if(boundIt != oneBoundary.end() - 1 && (*eIt) -> start == (*boundIt) && (*eIt) -> end == *(boundIt + 1)){
                    boundaryEdges.push_back(*eIt);
                } else if(boundIt == oneBoundary.end() - 1 && (*eIt) -> start == (*boundIt) && (*eIt) -> end == *(oneBoundary.begin())){
                    boundaryEdges.push_back(*eIt);
                }
            }
        }
    }
    vector<Halfedge*>::iterator edgeIt1;
    vector<Halfedge*>::iterator edgeIt2;
    for( edgeIt1 = boundaryEdges.begin(); edgeIt1 < boundaryEdges.end(); edgeIt1 ++){
        for(edgeIt2 = edgeIt1 +1; edgeIt2 < boundaryEdges.end(); edgeIt2++){
            if(((*edgeIt1)->start == (*edgeIt2)->start) &&((*edgeIt1)->end != (*edgeIt2)->end)){

                (*edgeIt1)->mobiusBoundary = *edgeIt2;
                (*edgeIt2)->mobiusBoundary = *edgeIt1;

            } else if (((*edgeIt1)->end == (*edgeIt2)->end) &&((*edgeIt1)->start != (*edgeIt2)->start)){

                (*edgeIt1)->mobiusBoundary = *edgeIt2;
                (*edgeIt2)->mobiusBoundary = *edgeIt1;

            } else if ((*edgeIt1)->start == (*edgeIt2)->end){

                (*edgeIt1)->previousBoundary = *edgeIt2;
                (*edgeIt2)->nextBoundary = *edgeIt1;

            } else if ((*edgeIt1)->end == (*edgeIt2)->start){

                (*edgeIt1)->nextBoundary = *edgeIt2;
                (*edgeIt2)->previousBoundary = *edgeIt1;

            }
        }
    }
    for(eIt = boundaryEdges.begin(); eIt < boundaryEdges.end(); eIt ++) {

        if((*eIt) -> mobiusSibling == NULL) {
            (*eIt) -> isSharp = true;
        }
    }
}