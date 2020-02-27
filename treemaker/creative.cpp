/*******************************************************************************
File:         creative.cpp
Project:      TreeMaker 5.x
Purpose:      Implementation file for testing the TreeMaker model (no GUI)
Author:       Piper Vasicek
Modified by:
Created:      2020-02-19
Copyright:    ©2020 Piper Vasicek. All Rights Reserved.
*******************************************************************************/

// standard libraries
#include <iostream>
#include <fstream>
#include <string>
#include <ctime>

using namespace std;

// TreeMaker model classes
#include "tmModel.h"
#include "tmNLCO.h"

static const string testdir =
  "C:/Users/Piper/Desktop/treemakertest/tmModelTester/";

/*****
Read the tree in from a file. The previous tree will be overwritten. Note that
the path to the directory of test files is hard-coded; users should change the
value of testdir to reflect their own file organization.
*****/
void DoReadFile(tmTree* theTree, const string& filename)
{
  string fullname = testdir + filename;
  cout << "Reading in file " << fullname << endl;
  ifstream fin(fullname.c_str());
  if (!fin.good()) {
    cout << "Unable to find file " << fullname << endl;
    exit(1);
  }
  try {
    theTree->GetSelf(fin);
  }
  catch(...) {
    cout << "Unexpected exception reading file " << fullname << endl;
    exit(1);
  }
}

void creasePatternToSVG(char* name)
{
    if(!name) return;
    const string filename(name);
    tmTree* theTree = new tmTree();
    DoReadFile(theTree, filename);
    cout << "Optimizing scale of " << filename << "..." << endl;
    tmNLCO* theNLCO = tmNLCO::MakeNLCO();
#ifdef tmUSE_CFSQP
  // set best algorithm for this type of optimization
  tmNLCO_cfsqp* theNLCO_cfsqp = dynamic_cast<tmNLCO_cfsqp*>(theNLCO);
  if (theNLCO_cfsqp) theNLCO_cfsqp->SetCFSQPMode(110);
#endif
  tmScaleOptimizer* theOptimizer = new tmScaleOptimizer(theTree, theNLCO);
  theOptimizer->Initialize();

    try {
        theOptimizer->Optimize();
    }
    catch (tmNLCO::EX_BAD_CONVERGENCE ex) {
        cout << "Scale optimization failed with result code " << ex.GetReason() << endl;
    }
    catch(tmScaleOptimizer::EX_BAD_SCALE) {
        cout << "Scale optimization failed with scale too small." << endl;
    }
    delete theOptimizer;
    delete theNLCO;
    cout << "BuildingPolys" << endl;
    theTree->BuildPolysAndCreasePattern();
    cout << "Polys Built" << endl;
    ofstream myfile2;
    myfile2.open (testdir+"example.tmd5");
    theTree->PutSelf(myfile2);
    myfile2.close();
    tmDpptrArray<tmCrease> theCreases = theTree->GetCreases();
    int n = theTree->GetNumCreases();
    ofstream myfile;
    myfile.open(testdir+"example.svg");
    myfile << "<svg xmlns=\"http://www.w3.org/2000/svg\">" << endl;
    string color = "#CCCCCC";
    int total_creases = 0;
    int total_borders = 0;
    for (std::size_t j = 0; j < n; ++j) {
        cout << theCreases[j]->GetKind() << "-";
        cout << theCreases[j]->GetFold() << endl;
        switch(theCreases[j]->GetFold()) {
          case tmCrease::VALLEY:
            color = "#FF0000";
            break;
          case tmCrease::MOUNTAIN:
              color="#0000FF";
              break;
          case tmCrease::BORDER:
            color="#000000";
            ++total_borders;
            break;
          case tmCrease::FLAT:
            color="#CCCCCC";
            break;
        }
        if(color!="") {
            ++total_creases;
            tmDpptrArray<tmVertex> theVertices = theCreases[j]->GetVertices();
            tmPoint p1 = theVertices[0]->GetLoc();
            tmPoint p2 = theVertices[1]->GetLoc();
            tmFloat x1 = p1.x;
            tmFloat y1 = p1.y;
            tmFloat x2 = p2.x;
            tmFloat y2 = p2.y;
            myfile << "<line file=\"none\" stroke=\"";
            myfile << color;
            myfile << "\" stroke-miterlimit=\"10\" x1=\"";
            myfile << (x1*2000);
            myfile << "\" y1=\"";
            myfile << (y1*2000);
            myfile << "\" x2=\"";
            myfile << (x2*2000);
            myfile << "\" y2=\"";
            myfile << (y2*2000);
            myfile << "\" />" << endl;
        }
    }
    if(total_borders<1) {
        myfile << "<rect x=\"0\" y=\"0\" fill=\"#FFFFFF\" stroke=\"#000000\" stroke-miterlimit=\"10\" width=\"2000\" height=\"2000\" />" << endl;
    }
    if(total_creases<1){
        for (std::size_t j = 0; j < n; ++j) {
            string color = "#CCCCCC";
            switch(theCreases[j]->GetKind()) {
              case tmCrease::RIDGE:
                color = "#FF0000";
              case tmCrease::GUSSET:
                color = "#0000FF";
                break;
            }
            tmDpptrArray<tmVertex> theVertices = theCreases[j]->GetVertices();
            tmPoint p1 = theVertices[0]->GetLoc();
            tmPoint p2 = theVertices[1]->GetLoc();
            tmFloat x1 = p1.x;
            tmFloat y1 = p1.y;
            tmFloat x2 = p2.x;
            tmFloat y2 = p2.y;
            myfile << "<line file=\"none\" stroke=\"";
            myfile << color;
            myfile << "\" stroke-miterlimit=\"10\" x1=\"";
            myfile << (x1*2000);
            myfile << "\" y1=\"";
            myfile << (y1*2000);
            myfile << "\" x2=\"";
            myfile << (x2*2000);
            myfile << "\" y2=\"";
            myfile << (y2*2000);
            myfile << "\" />" << endl;
        }
    }
    myfile << "</svg>" << endl;
    myfile.close();
    cout << "File Closed"<<endl;
    cout << theTree->HasFullCP();
}

/*****
Peform optimizations on several files using the templated optimization engine.
*****/
void DoSeveralOptimizations()
{
  // SCALE OPTIMIZATION

  // Read in a test structure from a file and do an optimization.
  //Then convert to SVG file
  creasePatternToSVG("tmModelTester_2.tmd5");
}

/*****
Main Program
*****/
int main()
{
  // Initialize our dynamic type system
  tmPart::InitTypes();

  // Do the optimization with all four engines

  DoSeveralOptimizations();


}

