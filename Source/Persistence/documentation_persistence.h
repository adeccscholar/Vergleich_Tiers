/**
  \file
  \brief  file for documentation of subprocess of persistence layer (database) 
          for the training project GeoData
  \details This file contains the related pages and informations to the definition and implementation
           of subprocesses for the database access for the project. We used the database layer which
		   we defined before.
  \author Volker Hillmann
  \date 22.08.20203 file created
  \version Version 2.0
  \since Version 2.0
  \copyright Copyright &copy; 2023. adecc Systemhaus GmbH
*/




/**
\dir Persistence
\brief directory with the sources for the persistence layer (database) for the project 
*/

/**
 \page pgPersistence subprocess of the persistance layer
 \brief Description for the subprocess of the persistance layer (database)
 \tableofcontents
 \section secDescriptions Description
 \section secParts
 \subsection secCreate methods to create database structure
 \see ref="#Process_Reader_Create"
 \subsection secSystem methods to work with entities of the system tie
 \subsection secOverview methods to create overview statements
 \section secFigureDatabase Figure of classes
 \details The following figure shows the logical structure of the classes for the database interface, 
          without taking into account any templates and merging 
  \dot 
    digraph persistence_classes {
	label="classes for persistence layer";
	labelloc = "t"; 
    node [shape=record, fontname=Helvetica, fontsize=10];
    OperationsBase [label="TOperationsBase" URL="\ref #TOperationsBase"];
    Process_Reader [label="TProcess_Reader" URL="\ref #TProcess_Reader"];
	 { rank=same
    Process_Reader_Create [label="TProcess_Reader_Create" URL="\ref #TProcess_Reader_Create"];
    Process_Reader_System [label="TProcess_Reader_System" URL="\ref #TProcess_Reader_System"];
    Process_Reader_Overview [label="TProcess_Reader_Overview" URL="\ref #TProcess_Reader_Overview"];
    }
    Process_Reader_Impl [label="TProcess_Reader_Impl" URL="\ref #TProcess_Reader_Impl"];
    { rank=same
    Process_Reader_Create_Impl [label="TProcess_Reader_Create_Impl" URL="\ref #TProcess_Reader_Create_Impl"];
    Process_Reader_System_Impl [label="TProcess_Reader_System_Impl" URL="\ref #TProcess_Reader_System_Impl"];
    Process_Reader_Overview_Impl [label="TProcess_Reader_Overview_Impl" URL="\ref #TProcess_Reader_Overview_Impl"];
    }
    OperationsBase -> Process_Reader [label="virtual", fontname=Helvetica, fontsize=8, dir="back", arrowhead="open"];
    Process_Reader -> Process_Reader_Create [label="virtual", fontname=Helvetica, fontsize=8, dir="back", arrowhead="open", len=1];
    Process_Reader_Create -> Process_Reader_Create_Impl [label="virtual", fontname=Helvetica, fontsize=8, dir="back", arrowhead="open"];
    Process_Reader -> Process_Reader_System [label="virtual", fontname=Helvetica, fontsize=8, dir="back", arrowhead="open", len=1];
    Process_Reader_System -> Process_Reader_System_Impl [label="virtual", fontname=Helvetica, fontsize=8, dir="back", arrowhead="open"];
    Process_Reader -> Process_Reader_Overview [label="virtual", fontname=Helvetica, fontsize=8, dir="back", arrowhead="open", len=1];
    Process_Reader_Overview -> Process_Reader_Overview_Impl [label="virtual", fontname=Helvetica, fontsize=8, dir="back", arrowhead="open"];
    Process_Reader -> Process_Reader_Impl [label="virtual", fontname=Helvetica, fontsize=8, dir="back", arrowhead="open", minlen=2];
	Process_Reader_Impl -> Process_Reader_Create_Impl [label="virtual", fontname=Helvetica, fontsize=8, dir="back", arrowhead="open"];
	Process_Reader_Impl -> Process_Reader_System_Impl [label="virtual", fontname=Helvetica, fontsize=8, dir="back", arrowhead="open"];
	Process_Reader_Impl -> Process_Reader_Overview_Impl [label="virtual", fontname=Helvetica, fontsize=8, dir="back", arrowhead="open"];
    }
  \enddot
  \author Volker Hillmann
  \date 01.09.2023
*/