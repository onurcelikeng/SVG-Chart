#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/xmlschemastypes.h> 
#include <math.h>

#define PI 3.14159265358979323846
#define M "M"
#define L "L"
#define A "A"
#define Z "Z"
#define vir ","
#define spacePie " "
#define one "1"


typedef struct Chartgendata
{
	char *charttitle;
}Chartgendata;


typedef struct Canvas
{
	char *length;
	char *width;
	char *backcolor;
}Canvas;


typedef struct Axis
{
	char *X_name;
	char *Y_name;
	char *X_forecolor;
	char *Y_forecolor;
}Axis;


typedef struct Xset
{
	char *X_data[12];
}Xset;


typedef struct Yset
{
	char *unit;
	char *name;
	char *showvalue;
	char *fillcolor;
	char *Y_data[30];
}Yset;


Yset ysetArray[30];
bool flag = true;
char *input, *output, *validation, *type;
bool inputControl(int length, char *array[]);
int sameInputController(int length, char *array[], char *value);
char* concat(char *s1, char *s2);
bool XMLControl(char *inputFile, char *validationFile);
void ReadFile(char *inputFile);
void ParseXMLFile(xmlNode *a_node);
void FreeMemory();
int getMaxValue();
void DrawGraph(char *outputFile);
void DrawBarChart(char *outputFile);
void DrawPieChart(char *outputFile);

// Global variables
int cityCounter = 0;
int yDataCounter = 0;
int monthsCounter = 0;
int maxValue = 0;

// Global Struct variables
Chartgendata chartgendata;
Canvas canvas;
Axis axis;
Xset xset;
Yset yset;

void main(int argc, char *argv[])
{
	if(inputControl(argc, argv) == true)
	{
		if(XMLControl(input, validation) == true)
		{
			ReadFile(input);

			if(strcmp(type, "line") == 0) DrawGraph(output);
			if(strcmp(type, "bar") == 0) DrawBarChart(output);
			if(strcmp(type, "pie") == 0) DrawPieChart(output);

			FreeMemory();
		}
	}

	else if(argc != 2)
	{
		printf("Incorrect syntax.\n");
	}
}


bool inputControl(int length, char *array[])
{
	// for help screen
	if(length == 2 && sameInputController(length, array, "-h") == 1)
	{
		printf("[-i <input filename>] [-o <output filename>] [-v <xsd validation file>] [-t <type>] [-h]\n");
		printf("<type> 'line' | 'pie' | 'bar'\n");

		flag = false;
	}

	else if(length == 9 && sameInputController(length, array, "-i") == 1 && sameInputController(length, array, "-o") == 1 && sameInputController(length, array, "-v") == 1 && sameInputController(length, array, "-t") == 1)
	{
		int i;

		//split for file name and commands.
		for(i = 0; i < length; i++)
		{
			if(strcmp(array[i], "-i") == 0) input = array[i+1];
			else if(strcmp(array[i], "-o") == 0) output = array[i+1];
			else if(strcmp(array[i], "-v") == 0) validation = array[i+1];
			else if(strcmp(array[i], "-t") == 0) type = array[i+1];
		}

		// type of output.svg
		if(strcmp(type, "line") == 0 || strcmp(type, "bar") == 0 || strcmp(type, "pie") == 0)
		{
			input = concat(input, ".xml");
			output = concat(output, ".svg");
			validation = concat(validation, ".xsd");

			flag = true;
		}

		// for other wrong command
		else
		{
			flag = false;
		}
	}

	else
	{
		flag = false;
	}

	return flag;
}


int sameInputController(int length, char *array[], char *value)
{
	// this function is prevented same paremeters. for example -i inputname -i outputname...
	// return value of this function must be 1.
	// if return value 0, there is no parameter.
	// if return value greater than 1, there is a lot of same parameters.
	
	int counter = 0;

	int i;

	for(i = 0; i < length; i++)
	{
		if(strcmp(array[i], value) == 0) counter++;
	}
	
	return counter;
}


char* concat(char *s1, char *s2)
{
	// provide cancot operations.
	
	size_t len1 = strlen(s1);
	size_t len2 = strlen(s2);

	char *result = malloc(len1 + len2 + 1);

	memcpy(result, s1, len1);
	memcpy(result+len1, s2, len2+1);

	return result;
}


bool XMLControl(char *inputFile, char *validationFile)
{
	// provide xml validations.

	char *XMLFileName = inputFile;
	char *XSDFileName = validationFile;

	xmlDocPtr doc;
	xmlSchemaPtr schema = NULL; 
	xmlSchemaParserCtxtPtr ctxt; 

	xmlLineNumbersDefault(1);

	ctxt = xmlSchemaNewParserCtxt(XSDFileName); 
	schema = xmlSchemaParse(ctxt); 
	xmlSchemaFreeParserCtxt(ctxt); 

	doc = xmlReadFile(XMLFileName, NULL, 0); 

	if (doc == NULL) 
	{ 
		fprintf(stderr, "Could not parse %s\n", XMLFileName); 
	} 
	
	else 
	{ 
		xmlSchemaValidCtxtPtr ctxt; 
		int ret; 

		ctxt = xmlSchemaNewValidCtxt(schema); 
		ret = xmlSchemaValidateDoc(ctxt, doc); 
		
		if (ret == 0) 
		{
			flag = true; 
		} 
		
		else if (ret > 0) 
		{ 
			printf("%s fails to validate\n", XMLFileName);
			flag = false; 
		} 
		
		else 
		{ 
			printf("%s validation generated an internal error\n", XMLFileName); 
			flag = false;
		} 
		
		xmlSchemaFreeValidCtxt(ctxt); 
		xmlFreeDoc(doc);		
	} 

	if(schema != NULL) 
	{
		xmlSchemaFree(schema); 
	}

	xmlSchemaCleanupTypes(); 
	xmlCleanupParser(); 
	xmlMemoryDump(); 

	return flag;
}


void ReadFile(char *inputFile)
{
	xmlDoc *document = NULL;
	xmlNode *rootElement = NULL;

	document = xmlReadFile(inputFile, NULL, 0);

	if(document == NULL)
	{
		printf("Error : could nor parse file %s\n", inputFile);
	}

	else
	{
		rootElement = xmlDocGetRootElement(document);

		ParseXMLFile(rootElement);

		xmlFreeDoc(document);
	}

	xmlCleanupParser();
}


void ParseXMLFile(xmlNode *a_node)
{
	xmlNode *cur_node = NULL;

	for (cur_node = a_node; cur_node; cur_node = cur_node->next) 
	{
		if (cur_node->type == XML_ELEMENT_NODE)
		{
			if(strcmp(cur_node->name, "charttitle") == 0)
			{
				chartgendata.charttitle = (char*)calloc(sizeof(strlen(cur_node->children->content) + 1),1);
				strcpy(chartgendata.charttitle, cur_node->children->content);		
			}

			if(strcmp(cur_node->name, "length") == 0 && strcmp(cur_node->parent->name, "canvas") == 0)
			{
				canvas.length = (char*)calloc(sizeof(strlen(cur_node->children->content) + 1),1);
				strcpy(canvas.length, cur_node->children->content);		
			}

			if(strcmp(cur_node->name, "width") == 0 && strcmp(cur_node->parent->name, "canvas") == 0)
			{
				canvas.width = (char*)calloc(sizeof(strlen(cur_node->children->content) + 1),1);
				strcpy(canvas.width, cur_node->children->content);		
			}

			if(strcmp(cur_node->name, "backcolor") == 0 && strcmp(cur_node->parent->name, "canvas") == 0)
			{
				canvas.backcolor = (char*)calloc(sizeof(strlen(cur_node->children->content) + 1),1);
				strcpy(canvas.backcolor, "#");
				strcat(canvas.backcolor, cur_node->children->content);
			}

			if(strcmp(cur_node->name, "name") == 0 && strcmp(cur_node->parent->name, "Xaxis") == 0)
			{
				axis.X_name = (char*)calloc(sizeof(strlen(cur_node->children->content) + 1),1);
				strcpy(axis.X_name, cur_node->children->content);
			}	

			if(strcmp(cur_node->name, "name") == 0 && strcmp(cur_node->parent->name, "Yaxis") == 0)
			{	
				axis.Y_name = (char*)calloc(sizeof(strlen(cur_node->children->content) + 1),1);	
				strcpy(axis.Y_name, cur_node->children->content);
			}	

			if(strcmp(cur_node->name, "forecolor") == 0 && strcmp(cur_node->parent->name, "Yaxis") == 0)
			{
				axis.Y_forecolor = (char*)calloc(sizeof(strlen(cur_node->children->content) + 1),1);	
				strcpy(axis.Y_forecolor, "#");	
				strcat(axis.Y_forecolor, cur_node->children->content);
			}

			if(strcmp(cur_node->name, "forecolor") == 0 && strcmp(cur_node->parent->name, "Xaxis") == 0)
			{
				axis.X_forecolor = (char*)calloc(sizeof(strlen(cur_node->children->content) + 1),1);	
				strcpy(axis.X_forecolor, "#");	
				strcat(axis.X_forecolor, cur_node->children->content);
			}

			if(strcmp(cur_node->name, "xdata") == 0 && strcmp(cur_node->parent->name, "Xset") == 0)
			{			
				xset.X_data[monthsCounter] = (char*)calloc(sizeof(strlen(cur_node->children->content) + 1),1);	
				strcpy(xset.X_data[monthsCounter], cur_node->children->content);	
				monthsCounter++;
			}

			if(strcmp(cur_node->name, "Yset") == 0)
			{	
				// define attributes of current node in while loop

				yDataCounter = 0;
				xmlAttr* attribute = cur_node->properties;
				bool showValueControl = false;
				
				while(attribute)
				{
					xmlChar* value = xmlNodeListGetString(cur_node->doc, attribute->children, 1);

					if(strcmp(attribute->name, "unit") == 0)
					{
						ysetArray[cityCounter].unit = (char*)calloc(sizeof(strlen(cur_node->children->content) + 1),1);	
						strcpy(ysetArray[cityCounter].unit, value);
					}

					if(strcmp(attribute->name, "name") == 0)
					{
						ysetArray[cityCounter].name = (char*)calloc(sizeof(strlen(cur_node->children->content) + 1),1);	
						strcpy(ysetArray[cityCounter].name, value);
					}

					if(strcmp(attribute->name, "showvalue") == 0)
					{
						showValueControl = true;
						ysetArray[cityCounter].showvalue = (char*)calloc(sizeof(strlen(cur_node->children->content) + 1),1);	
						strcpy(ysetArray[cityCounter].showvalue, value);
					}
					if(showValueControl == false)
					{
						ysetArray[cityCounter].showvalue = (char*)calloc(sizeof(strlen(cur_node->children->content) + 1),1);	
						strcpy(ysetArray[cityCounter].showvalue, "no");
					}

					if(strcmp(attribute->name, "fillcolor") == 0)
					{
						ysetArray[cityCounter].fillcolor = (char*)calloc(sizeof(strlen(cur_node->children->content) + 1),1);	
						strcpy(ysetArray[cityCounter].fillcolor, "#");
						strcat(ysetArray[cityCounter].fillcolor, value);
					}

					xmlFree(value);
					attribute = attribute->next;
				}
				
				cityCounter++;
			}

			if(strcmp(cur_node->name, "ydata") == 0 && strcmp(cur_node->parent->name, "Yset") == 0)
			{			
				ysetArray[cityCounter - 1].Y_data[yDataCounter] = (char*)calloc(sizeof(strlen(cur_node->children->content) + 1),1);	
				strcpy(ysetArray[cityCounter - 1].Y_data[yDataCounter], cur_node->children->content);	
				yDataCounter++;
			}

		}	
		ParseXMLFile(cur_node->children);
	}
}


void FreeMemory()
{
	free(input);
	free(output);
	free(validation);
	free(canvas.length);
	free(canvas.width);
	free(canvas.backcolor);
	free(axis.X_name);
	free(axis.Y_name);
	free(axis.Y_forecolor);
	free(axis.X_forecolor);
	free(chartgendata.charttitle);

	int i;
	for(i = 0; i < cityCounter; i++){
		int k;
		for (k= 0; k < yDataCounter; k++)
		{
			free(ysetArray[i].Y_data[k]);
		}

	}

	for (i= 0; i < monthsCounter; i++)
	{
		free(xset.X_data[i]);			
	}
}


int getMaxValue()
{
	// find a maximum value in all y set data element
	// for bar chart and line chart.

	int i;
	int maximum = 0;
	
	for(i = 0; i < cityCounter; i++)
	{
		int j;
		for(j = 0; j < yDataCounter; j++)
		{
			int value = atoi(ysetArray[i].Y_data[j]);
			
			if(maximum <= value)
			{
				maximum = value;
			}
		}
	}
	
	return maximum;
}


void DrawGraph(char *outputFile)
{
	//canvas is divided 6 part and length of each part is grid.
	double grid = atof(canvas.length) / 6;
	int _grid = grid / 6;
	
	//define dynamic text size
	char textSize[_grid * 6 * 11 / 400];
	sprintf(textSize, "%.2f", grid * 6 * 11 / 400);
	double increaseRate = getMaxValue() / (monthsCounter - 2);

	xmlDocPtr doc = NULL;
	xmlNodePtr rootNode = NULL, node = NULL, children = NULL;
	doc = xmlNewDoc(BAD_CAST"1.2");
	
	//create root node
	rootNode = xmlNewNode(NULL, BAD_CAST"html"); xmlDocSetRootElement(doc, rootNode);
	xmlNewChild(rootNode, NULL, BAD_CAST"children", NULL);
	node = xmlNewChild(rootNode, NULL, BAD_CAST"svg", NULL);
	
	//define size of canvas
	xmlNewProp(node, BAD_CAST"height", BAD_CAST canvas.length); 
	xmlNewProp(node, BAD_CAST"width", BAD_CAST canvas.width);

	//define backcolor of output.svg; that was used rectengle elements.
	children = xmlNewChild(node, NULL, BAD_CAST"rect", NULL);
	xmlNewProp(children,BAD_CAST"width",BAD_CAST canvas.width);
	xmlNewProp(children,BAD_CAST"height",BAD_CAST canvas.length);
	xmlNewProp(children,BAD_CAST"fill",BAD_CAST canvas.backcolor);

	//define header text of chart elements. and proporties of header.
	char horizontal[_grid * 3]; sprintf(horizontal, "%.2f", grid * 5 / 2.5);
	char vertical[_grid * 1 / 2]; sprintf(vertical, "%.2f", grid * 1 / 2);
	char headerTextSize[_grid * 6 * 20 / 400];
	sprintf(headerTextSize, "%.2f", grid * 6 * 20 /400);
	children = xmlNewChild(node, NULL, BAD_CAST"text", BAD_CAST chartgendata.charttitle);
	xmlNewProp(children, BAD_CAST"x", BAD_CAST horizontal); xmlNewProp(children,BAD_CAST"y", BAD_CAST vertical);
	xmlNewProp(children, BAD_CAST"font-size",BAD_CAST headerTextSize);
	xmlNewProp(children, BAD_CAST"style", BAD_CAST "font-weight:bold; stroke:black; fill:#0099FF stroke-width:0.5");

	char _Xaxis[sizeof(grid * 5)]; sprintf(_Xaxis, "%.2f", grid * 5.3);
	char _Yaxis[sizeof(grid)]; sprintf(_Yaxis, "%.2f", grid);
	char _yLine[sizeof(grid * 2 / 4)]; sprintf(_yLine, "%.2f", grid * 2 / 4);

	double part = grid * 5 / (monthsCounter + 1);
	double xLine = grid * 5;

	double item = 0; int i;

	for(i = 0; i < monthsCounter - 1; i++)
	{		
		char _xLine[sizeof(xLine)]; sprintf(_xLine, "%.2f", xLine);
		char _item[sizeof(item)]; sprintf(_item, "%.0f", item);

		// display Y data elements on Y axis
		children = xmlNewChild(node, NULL, BAD_CAST"text", BAD_CAST _item);
		xmlNewProp(children,BAD_CAST"x", BAD_CAST _yLine); 
		xmlNewProp(children,BAD_CAST"y",BAD_CAST _xLine);
		xmlNewProp(children,BAD_CAST"font-size", BAD_CAST textSize);
		xmlNewProp(children,BAD_CAST"fill", BAD_CAST"blue");

		// horizontal dashed lines 
		children = xmlNewChild(node,NULL,BAD_CAST"line", NULL);
		xmlNewProp(children,BAD_CAST"x1",BAD_CAST _Xaxis); xmlNewProp(children,BAD_CAST"y1",BAD_CAST _xLine);
		xmlNewProp(children,BAD_CAST"x2",BAD_CAST _Yaxis); xmlNewProp(children,BAD_CAST"y2",BAD_CAST _xLine);
		xmlNewProp(children,BAD_CAST"style",BAD_CAST"stroke:rgb(255,0,0);stroke-dasharray: 10,5;stroke-width:0.5");

		xLine = xLine - part; //part is decreasea amount of between months
		item = item + increaseRate; // inscreaseRate is increase amount of between y data elements
	}

	// define Y axis name and X axis name elements
	for(i = 1; i < 3; i++)
	{
		char margin_Yname[sizeof(grid * 3 / 4)]; sprintf(margin_Yname, "%.2f", grid * 3 / 4);
		char margin_Xname[sizeof(grid * 5 + 5)]; sprintf(margin_Xname, "%.2f", grid * 5 + 5);

		children = xmlNewChild(node,NULL, BAD_CAST"text", BAD_CAST((i % 2) ? axis.Y_name : axis.X_name));
		xmlNewProp(children,BAD_CAST"x", BAD_CAST((i % 2) ? margin_Yname : margin_Xname));
		xmlNewProp(children,BAD_CAST"y", BAD_CAST((i % 2) ? margin_Yname : margin_Xname));
		xmlNewProp(children,BAD_CAST"font-size",BAD_CAST textSize);
		xmlNewProp(children,BAD_CAST"fill", BAD_CAST((i % 2) ? axis.Y_forecolor : axis.X_forecolor));
	}

	 _yLine[sizeof(grid * 5 + 10)]; sprintf(_yLine, "%.2f", grid * 5 + 10);
	 xLine = grid; char _xLine[sizeof(grid)];
	 part = grid * 5 / (monthsCounter +1); //space of between each months

	// display x data elements on X axis
	for(i = 0; i < monthsCounter; i++)
	{
		sprintf(_xLine, "%.2f", xLine);

		children = xmlNewChild(node,NULL,BAD_CAST"text", BAD_CAST(xset.X_data[i]));
		xmlNewProp(children,BAD_CAST"x",BAD_CAST _xLine); xmlNewProp(children,BAD_CAST"y",BAD_CAST _yLine);
		xmlNewProp(children,BAD_CAST"font-size",BAD_CAST textSize);
		xmlNewProp(children,BAD_CAST"fill",BAD_CAST"black");
		xmlNewProp(children,BAD_CAST"style",BAD_CAST"writing-mode: tb;");

		xLine = xLine + part; //give a space between each mounts as part value
	}

	// define and display all polyline element points.
	int j;
	for(j = 0; j < cityCounter; j++)
	{
		part = (double)atoi(canvas.length) * 500 / (monthsCounter + 1) / 600;
	
		if(monthsCounter == 1) xLine = grid * 1.00;
		if(monthsCounter == 2) xLine = grid * 2.67;
		if(monthsCounter == 3) xLine = grid * 3.51;
		if(monthsCounter == 4) xLine = grid * 4.01;
		if(monthsCounter == 5) xLine = grid * 4.35;
		if(monthsCounter == 6) xLine = grid * 4.57;
		if(monthsCounter == 7) xLine = grid * 4.72;
		if(monthsCounter == 8) xLine = grid * 4.88;
		if(monthsCounter == 9) xLine = grid * 5.02;
		if(monthsCounter == 10) xLine = grid * 5.06;
		if(monthsCounter == 11) xLine = grid * 5.15;
		if(monthsCounter == 12) xLine = grid * 5.23;

		char *path = ""; //polyline element points.

		for(i = 0; i < monthsCounter; i++)
		{
			char _xLine[sizeof(xLine)]; sprintf(_xLine, "%.2f", xLine);

			double girilen = atoi(ysetArray[j].Y_data[monthsCounter - 1 - i]);
			double location = (double)atoi(canvas.length) * 500 / (monthsCounter + 1) / 600 * girilen / increaseRate;
			double loc =  grid * 5 - location;
			char _location[sizeof(loc)]; sprintf(_location, "%.2f", loc);

			path = concat(path, _xLine); path = concat(path, ","); path = concat(path, _location); path = concat(path, " ");
			xLine = xLine - part;
		}
		
		char lineStroke[_grid * 6 * 2 / 400];
		sprintf(lineStroke, "%.2f", grid * 6 * 2 / 400);

		//polyline element
		children = xmlNewChild(node,NULL,BAD_CAST"polyline", NULL);
		xmlNewProp(children,BAD_CAST"points",BAD_CAST path);
		xmlNewProp(children,BAD_CAST"stroke",BAD_CAST ysetArray[j].fillcolor);
		xmlNewProp(children,BAD_CAST"stroke-width",BAD_CAST lineStroke);
		xmlNewProp(children,BAD_CAST"style",BAD_CAST"fill:none");
	}

	// display showvalue of points, if showvalue is active/true/yes
	for(j = 0; j < cityCounter; j++)
	{
		part = (double)atoi(canvas.length) * 500 / (monthsCounter + 1) / 600;
	
		if(monthsCounter == 1) xLine = grid * 1.00;
		if(monthsCounter == 2) xLine = grid * 2.67;
		if(monthsCounter == 3) xLine = grid * 3.51;
		if(monthsCounter == 4) xLine = grid * 4.01;
		if(monthsCounter == 5) xLine = grid * 4.35;
		if(monthsCounter == 6) xLine = grid * 4.57;
		if(monthsCounter == 7) xLine = grid * 4.72;
		if(monthsCounter == 8) xLine = grid * 4.88;
		if(monthsCounter == 9) xLine = grid * 5.02;
		if(monthsCounter == 10) xLine = grid * 5.06;
		if(monthsCounter == 11) xLine = grid * 5.15;
		if(monthsCounter == 12) xLine = grid * 5.23;

		for(i = 0; i < monthsCounter; i++)
		{
			char _xLine[sizeof(xLine)]; sprintf(_xLine, "%.2f", xLine);

			double girilen = atoi(ysetArray[j].Y_data[monthsCounter - 1 - i]);
			double location = (double)atoi(canvas.length) * 500 / (monthsCounter + 1) / 600 * girilen / increaseRate;
			double loc =  grid * 5 - location;
			char _location[sizeof(loc)]; sprintf(_location, "%.2f", loc);

			if(strcmp(ysetArray[j].showvalue, "yes") == 0)
			{
				char showvalueSize[_grid * 6 * 8 / 400]; sprintf(showvalueSize, "%.2f", grid * 6 * 8 / 400);

				children = xmlNewChild(node, NULL, BAD_CAST"text", BAD_CAST ysetArray[j].Y_data[monthsCounter - i -1]);
				xmlNewProp(children,BAD_CAST"x",BAD_CAST _xLine); xmlNewProp(children,BAD_CAST"y",BAD_CAST _location);
				xmlNewProp(children,BAD_CAST"font-size",BAD_CAST showvalueSize);
				xmlNewProp(children,BAD_CAST"fill",BAD_CAST "black");
			}
			
			xLine = xLine - part;
		}
	}

	char textLeft[sizeof(grid * 5)]; sprintf(textLeft, "%.2f", grid * 5);
	double textTop = grid * 1 / 2;
	char _textTop[sizeof(textTop)];

	// display city name
	for(i = 0; i < cityCounter; i++)
	{
		sprintf(_textTop, "%.2f", textTop);

		children = xmlNewChild(node,NULL,BAD_CAST"text", BAD_CAST ysetArray[i].name);
		xmlNewProp(children,BAD_CAST"x",BAD_CAST textLeft); xmlNewProp(children,BAD_CAST"y",BAD_CAST _textTop);
		xmlNewProp(children,BAD_CAST"font-size",BAD_CAST textSize); xmlNewProp(children,BAD_CAST"fill",BAD_CAST ysetArray[i].fillcolor);

		textTop = textTop + grid * 1 / 4;
	}

	// draw axises
	for(i = 1; i < 3; i++)
	{
		char margin[sizeof(grid)]; sprintf(margin, "%.2f", grid);
		char axis[sizeof(5 * grid)]; sprintf(axis, "%.2f", 5 * grid);

		children = xmlNewChild(node,NULL,BAD_CAST"line", NULL);
		xmlNewProp(children,BAD_CAST"x1", BAD_CAST((i % 2) ? margin : axis)); 
		xmlNewProp(children,BAD_CAST"y1", BAD_CAST((i % 2) ? margin : axis));
		xmlNewProp(children,BAD_CAST"x2",BAD_CAST margin); 
		xmlNewProp(children,BAD_CAST"y2",BAD_CAST axis);
		xmlNewProp(children,BAD_CAST"style",BAD_CAST"stroke:rgb(255,0,0);stroke-width:2");
	}

	xmlSaveFormatFileEnc(outputFile, doc, "UTF-8", 1);
}


void DrawBarChart(char *outputFile) //char *outputFile
{
	xmlDocPtr doc = NULL;
	xmlNodePtr nodeSVG = NULL, nodeRect = NULL, nodeTspan = NULL, nodeLine = NULL,nodeG = NULL, nodeText = NULL;
	doc = xmlNewDoc(BAD_CAST"1.2");
	
	nodeSVG = xmlNewNode(NULL, BAD_CAST "svg");
	xmlNewProp(nodeSVG, BAD_CAST"height", BAD_CAST canvas.length); // length and width add from struct _canvas
	xmlNewProp(nodeSVG, BAD_CAST"width", BAD_CAST canvas.width);
	xmlNewProp(nodeSVG, BAD_CAST"viewbox", BAD_CAST "10 10 510 350"); // 10 10 510 350background nasıl değişecek??

	xmlDocSetRootElement(doc,nodeSVG);

	double barLength = atoi(canvas.length) / 1.33333;  
	double xPosition;
	double yPosition;
	double _xPosition = barLength * 25 / 150;
	char spaceX[10], sizeA[10],spaceY[10],_spaceX[10];
	yPosition = barLength * 10 / 150;
	xPosition = barLength * 100 / 150;
	double size = barLength*11/150;
	sprintf(sizeA, "%.2f", size);
	sprintf(spaceX, "%.2f", xPosition);
	sprintf(spaceY, "%.2f", yPosition);
 

	nodeRect = xmlNewChild(nodeSVG,NULL, BAD_CAST "rect",NULL);
	xmlNewProp(nodeRect,BAD_CAST"width",BAD_CAST canvas.length);
	xmlNewProp(nodeRect,BAD_CAST"height",BAD_CAST canvas.width);
	xmlNewProp(nodeRect,BAD_CAST"fill",BAD_CAST canvas.backcolor);

	nodeText = xmlNewChild(nodeSVG,NULL, BAD_CAST "text",BAD_CAST chartgendata.charttitle);
	xmlNewProp(nodeText,BAD_CAST"x",BAD_CAST spaceX);
	xmlNewProp(nodeText,BAD_CAST"y",BAD_CAST spaceY);
	xmlNewProp(nodeText,BAD_CAST"text-anchor",BAD_CAST "middle");
	xmlNewProp(nodeText,BAD_CAST"font-size",BAD_CAST sizeA);
	xmlNewProp(nodeText,BAD_CAST"font-weight",BAD_CAST "bold");
	xmlNewProp(nodeText,BAD_CAST"fill",BAD_CAST "#260A39");

	yPosition = barLength * 15 / 150;
	xPosition = barLength * 25 / 150;
	size = barLength*6/150;
	sprintf(sizeA, "%.2f", size);
	sprintf(spaceX, "%.2f", xPosition);
	sprintf(spaceY, "%.2f", yPosition);

//---------------------------------------------------- for kullanılacak
	strcat(axis.Y_name, "(");
	strcat(axis.Y_name, ysetArray[0].unit);	strcat(axis.Y_name, ")");

	nodeText = xmlNewChild(nodeSVG, NULL, BAD_CAST "text", BAD_CAST axis.Y_name);
	xmlNewProp(nodeText, BAD_CAST"x", BAD_CAST spaceX);
	xmlNewProp(nodeText, BAD_CAST"y", BAD_CAST spaceY);
	xmlNewProp(nodeText, BAD_CAST"text-anchor", BAD_CAST "middle");
	xmlNewProp(nodeText, BAD_CAST"font-size", BAD_CAST sizeA);
	xmlNewProp(nodeText, BAD_CAST"font-weight", BAD_CAST "bold");
	yPosition = barLength * 175 / 150;	sprintf(spaceY, "%.2f", yPosition);
	xPosition = barLength * 188 / 150;	sprintf(spaceX, "%.2f", xPosition);

	nodeText = xmlNewChild(nodeSVG, NULL, BAD_CAST "text", BAD_CAST axis.X_name);
	xmlNewProp(nodeText, BAD_CAST"x", BAD_CAST spaceX);
	xmlNewProp(nodeText, BAD_CAST"y", BAD_CAST spaceY);
	xmlNewProp(nodeText, BAD_CAST"text-anchor", BAD_CAST "middle");
	xmlNewProp(nodeText, BAD_CAST"font-size", BAD_CAST sizeA);
	xmlNewProp(nodeText, BAD_CAST"font-weight", BAD_CAST "bold");

	size = barLength*0.5/150;  sprintf(sizeA, "%.2f", size);
	nodeG = xmlNewChild(nodeSVG, NULL, BAD_CAST "g",NULL);
	xmlNewProp(nodeG,BAD_CAST"stroke-width",BAD_CAST sizeA);

//---------------------------------  BarsDraw  ------------------------------------------------

	int i;
	int j;
	double count = 0;
	double temp = 0;
	double spaceAllBar = barLength / monthsCounter / (cityCounter+1); /// tek şehir için


	for (j = 0; j < cityCounter; j++)
	{
		count= count + barLength*25/150;
		temp++;

		for (i = 0; i <yDataCounter; i++)
		{
			xPosition = (barLength * 25 / 150);	 sprintf(spaceX, "%.2f", xPosition);
			yPosition = (barLength * 175 / 150);  sprintf(spaceY, "%.2f", yPosition);

			sprintf(_spaceX, "%.2f", _xPosition);

			//line elements
			nodeLine = xmlNewChild(nodeSVG,NULL,BAD_CAST"line", NULL);
			xmlNewProp(nodeLine,BAD_CAST"x1",BAD_CAST spaceX);
			xmlNewProp(nodeLine,BAD_CAST"y1",BAD_CAST _spaceX);
			xmlNewProp(nodeLine,BAD_CAST"x2",BAD_CAST spaceY);
			xmlNewProp(nodeLine,BAD_CAST"y2",BAD_CAST _spaceX);
			xmlNewProp(nodeLine,BAD_CAST"style",BAD_CAST"stroke:rgb(0,0,0);stroke-dasharray:8,2 ;stroke-width:0.3");
			xmlNewProp(nodeLine, BAD_CAST"font-size", BAD_CAST sizeA);

			_xPosition = _xPosition + (barLength * 25 / 150);

			
			double barValue = atoi(ysetArray[j].Y_data[i]);
			double x_space = barLength / 6;
			double space = barLength + x_space; // toplam y eksen uzunluğu
			double barValues = (barLength * barValue) / getMaxValue(); // bar height
			double result = space - barValues;
			xPosition = count ;
			double barWidth = spaceAllBar;                  // 8 / 150;
			double size2;
			char bar[10], spaceY[10], spaceW[10],sizeA2[10];
			sprintf(bar, "%.2f", barValues);
			sprintf(spaceY, "%.2f", result);
			sprintf(spaceX, "%.2f", xPosition);
			sprintf(spaceW, "%.2f", barWidth);

			count = (spaceAllBar *  (cityCounter+1)) + count;
			
			
			nodeRect = xmlNewChild(nodeG, NULL, BAD_CAST "rect", NULL);
			xmlNewProp(nodeRect, BAD_CAST"x", BAD_CAST spaceX);
			xmlNewProp(nodeRect, BAD_CAST"y", BAD_CAST spaceY);
			xmlNewProp(nodeRect, BAD_CAST"width", BAD_CAST spaceW);
			xmlNewProp(nodeRect, BAD_CAST"height", BAD_CAST bar);
			xmlNewProp(nodeRect, BAD_CAST"fill", BAD_CAST ysetArray[j].fillcolor);
			xmlNewProp(nodeRect, BAD_CAST"stroke", BAD_CAST "black");


			if(strcmp(ysetArray[j].showvalue,"yes") == 0){

				size = xPosition + 30*barLength/150/(cityCounter*monthsCounter)*barLength/150;
				sprintf(sizeA, "%.2f", size);
				size2 = 12*barLength/150/(cityCounter);
				sprintf(sizeA2, "%.2f", size2);
			nodeText = xmlNewChild(nodeSVG, NULL, BAD_CAST "text", BAD_CAST ysetArray[j].Y_data[i]);
			xmlNewProp(nodeText, BAD_CAST"x", BAD_CAST sizeA);
			xmlNewProp(nodeText, BAD_CAST"y", BAD_CAST spaceY);
			xmlNewProp(nodeText, BAD_CAST"writing-mode", BAD_CAST "tb");
			xmlNewProp(nodeText, BAD_CAST"font-size", BAD_CAST sizeA2);
			xmlNewProp(nodeText, BAD_CAST"font-weight", BAD_CAST "bold");

			}


		}
		count = spaceAllBar*temp;
	}
//----------------------------------- ShowSquareSetName -----------------------------------------------

		xPosition = barLength * 160 / 150 ;
		yPosition = (barLength * 5) / 150;
		size = barLength*6.25/150;
		sprintf(spaceX, "%.2f", xPosition);
	    sprintf(spaceY, "%.2f", yPosition);
		sprintf(sizeA, "%.2f", size);

		for (i = 0; i < cityCounter; i++)
		{
			nodeRect = xmlNewChild(nodeG, NULL, BAD_CAST "rect", NULL);
			xmlNewProp(nodeRect, BAD_CAST"x", BAD_CAST spaceX);
			xmlNewProp(nodeRect, BAD_CAST"y", BAD_CAST spaceY);
			xmlNewProp(nodeRect, BAD_CAST"width", BAD_CAST sizeA);
			xmlNewProp(nodeRect, BAD_CAST"height", BAD_CAST sizeA);
			xmlNewProp(nodeRect, BAD_CAST"fill", BAD_CAST ysetArray[i].fillcolor); // array size da gelenin rengini basacak
			xmlNewProp(nodeRect, BAD_CAST"stroke", BAD_CAST "black");

			yPosition = yPosition + barLength * 7/ 150;	 sprintf(spaceY, "%.2f", yPosition);
		}

//---------------------------------  MonthsWrite  ------------------------------------------------

		size = barLength * 5 / 150;	 sprintf(sizeA, "%.2f", size);
		count =  (barLength * 25  / 150) + spaceAllBar;
		yPosition = barLength*177/150;	sprintf(spaceY, "%.2f", yPosition);

		nodeG = xmlNewChild(nodeSVG, NULL, BAD_CAST "g", NULL);
		xmlNewProp(nodeG, BAD_CAST"font-size", BAD_CAST sizeA);
		nodeText = xmlNewChild(nodeG, NULL, BAD_CAST "text", NULL);
		for(i = 0; i <monthsCounter; i++){

			xPosition = count;  sprintf(spaceX, "%.2f", xPosition);

			nodeTspan = xmlNewChild(nodeText, NULL, BAD_CAST "tspan", BAD_CAST xset.X_data[i]);
			xmlNewProp(nodeTspan, BAD_CAST"x", BAD_CAST spaceX);
			xmlNewProp(nodeTspan, BAD_CAST"y", BAD_CAST spaceY);
			xmlNewProp(nodeTspan, BAD_CAST"writing-mode", BAD_CAST "tb");
			xmlNewProp(nodeTspan, BAD_CAST"font-weight", BAD_CAST "bold");

			count = (spaceAllBar * (cityCounter+1)) + count;
		}

//---------------------------------- SalesWrite  -------------------------------------------------

	nodeText = xmlNewChild(nodeG,NULL, BAD_CAST "text", NULL);
	count = 0;
	int a = getMaxValue()/6;
	int gecici = a + getMaxValue();
	char geciciArr[10];
	size = barLength*6/150; sprintf(sizeA, "%.2f", size);
	
	for(i = 0; i < 6; i++){

		xPosition = barLength * 15 / 150;    sprintf(spaceX, "%.2f", xPosition);
		yPosition = (barLength * 25 / 150) + count;   sprintf(spaceY, "%.2f", yPosition);
		gecici = gecici - a;   sprintf(geciciArr, "%d", gecici);
		
		nodeTspan = xmlNewChild(nodeText, NULL, BAD_CAST "tspan", BAD_CAST geciciArr);
		xmlNewProp(nodeTspan, BAD_CAST"x", BAD_CAST spaceX);
		xmlNewProp(nodeTspan, BAD_CAST"y", BAD_CAST spaceY);
		xmlNewProp(nodeTspan, BAD_CAST"text-anchor", BAD_CAST "middle");
		xmlNewProp(nodeTspan, BAD_CAST"font-size", BAD_CAST sizeA);
		xmlNewProp(nodeTspan, BAD_CAST"font-weight", BAD_CAST "bold");

		count = count + 25 * barLength / 150;
	}
	
//-------------------------------------- Y_Set_NameWrite------------------------------------------------
	xPosition = barLength * 170 / 150;	sprintf(spaceX, "%.2f", xPosition);
	yPosition = barLength * 10 / 150;  sprintf(spaceY, "%.2f", yPosition);
    size = barLength*1/150;	 sprintf(sizeA, "%.2f", size);

	for(i = 0; i < cityCounter; i++){
		
		nodeText = xmlNewChild(nodeG,NULL, BAD_CAST "text", BAD_CAST ysetArray[i].name);
		xmlNewProp(nodeText, BAD_CAST"x", BAD_CAST spaceX);
		xmlNewProp(nodeText, BAD_CAST"y", BAD_CAST spaceY);
		xmlNewProp(nodeText, BAD_CAST"font-weight", BAD_CAST "bold");

		yPosition = (barLength * 7 / 150) + yPosition;  sprintf(spaceY, "%.2f", yPosition);
	}	

//----------------------------------------- LineDraw ----------------------------------
	xPosition= barLength/6;	 sprintf(spaceX,"%.2f",xPosition);
	_xPosition = barLength + 25*barLength/150;  sprintf(_spaceX,"%.2f",_xPosition);

    for (i = 1; i < 3; i++)
    {
		nodeLine = xmlNewChild(nodeSVG,NULL, BAD_CAST "line",NULL);
		xmlNewProp(nodeLine, BAD_CAST"x1", BAD_CAST ((i%2) ? spaceX :spaceX ));
		xmlNewProp(nodeLine, BAD_CAST"y1", BAD_CAST ((i%2) ? _spaceX :spaceX ));
		xmlNewProp(nodeLine, BAD_CAST"x2", BAD_CAST ((i%2) ? _spaceX :spaceX ));
		xmlNewProp(nodeLine, BAD_CAST"y2", BAD_CAST ((i%2) ? _spaceX :_spaceX ));
		xmlNewProp(nodeLine, BAD_CAST"stroke", BAD_CAST "black");
		xmlNewProp(nodeLine, BAD_CAST"stroke-width", BAD_CAST sizeA);

	}
	xmlSaveFormatFileEnc(outputFile, doc, "UTF-8", 1);
}


void DrawPieChart(char *outputFile)
{
	double getTotal(int ySet);
	double x_center = 0;
	double y_center = 0;
	double _radius = 0;
	double x_start = 0;
	double y_start = 0;
	double tempX = 0;
	double tempY = 0;
	double x_result = 0;
	double y_result = 0;
	double rating = 0;	
	double strChange =0;
	char result[400];
	char slice[100];
	char *colors[12] = {"#B80000", "#FF8000", "#FFFF00", "#00FF00", "#FF7A7A", "#3DFFFF", "#0000FF", "#8000FF", "#B80000" , "#666600", "#660066", "#FF47A3"};
	int ifBiggerThanHalf = 0;

		xmlDocPtr doc = NULL;
	xmlNodePtr rootNode = NULL, node = NULL, children = NULL;
	doc = xmlNewDoc(BAD_CAST"1.2");

	double editLength = atoi(canvas.length) * cityCounter; //editing length of canvas due to the number of cities.
	char edit[sizeof(editLength)];
	sprintf(edit, "%.2f", editLength); 

	rootNode = xmlNewNode(NULL, BAD_CAST "svg");
	xmlNewProp(rootNode, BAD_CAST "height", BAD_CAST edit);
	xmlNewProp(rootNode, BAD_CAST "width", BAD_CAST canvas.width);

	xmlDocSetRootElement(doc, rootNode);

	int i;
    int j;
    double length = atoi(canvas.length);

	children = xmlNewChild(rootNode, NULL, BAD_CAST "rect", NULL);
	xmlNewProp(children, BAD_CAST "height" , BAD_CAST edit);
	xmlNewProp(children, BAD_CAST "width" , BAD_CAST canvas.width);
	xmlNewProp(children, BAD_CAST "fill" , BAD_CAST canvas.backcolor);

	double _xTitle = length * 100 / 300; //printing title
	double _yTitle = length * 20 / 300;
	double _titleSize = length * 20 / 300;
	char xTitle[sizeof(_xTitle)],  yTitle[sizeof(_yTitle)], titleSize[sizeof(_titleSize)];
	sprintf(xTitle, "%.2f", _xTitle);
	sprintf(yTitle, "%.2f", _yTitle);
	sprintf(titleSize, "%.2f", _titleSize); 

	children = xmlNewChild(rootNode,NULL, BAD_CAST "text", BAD_CAST chartgendata.charttitle);
	xmlNewProp(children,BAD_CAST"x", BAD_CAST xTitle);
	xmlNewProp(children,BAD_CAST"y", BAD_CAST yTitle);
	xmlNewProp(children,BAD_CAST"font-size",BAD_CAST titleSize);
	xmlNewProp(children, BAD_CAST "font-weight", BAD_CAST "bold");

    //printing boxes, values and months
	double _xPosition = length * 241 / 300;
	double _yPosition = length * 60 / 300;
	double _xName = length * 150 / 300;
	double _yName = length * 50 / 300;
	double _space = length * 20 / 300;
	double _cube = length * 6.5 / 300;
	double _gap = length * 8 / 300;
	double _fontSize = length * 12 / 300;
	double _xValue = _xPosition - length * 30 / 300;
	double _yValue = length * 86 / 300;
	double _xSales = length * 170 / 300;
	double _ySales = length * 70 / 300;
	double _xMonths = length * 250 / 300;
	double _yMonths = length * 70 / 300;
	double _yMonthss = 0;
	_gap = _gap + _xPosition;
	char  cube[sizeof(_cube)], xPosition[sizeof(_xPosition)],gap[sizeof(_gap)], yPosition[sizeof(_yPosition)], fontSize[sizeof(_fontSize)];
	char xName[sizeof(_xName)], yName[sizeof(_yName)], xValue[sizeof(_xValue)], yValue[sizeof(_yValue)], xSales[sizeof(_xSales)], ySales[sizeof(_ySales)];
	char xMonths[sizeof(_xMonths)], yMonths[sizeof(_yMonths)], yMonthss[sizeof(_yMonthss)];
	sprintf(xPosition,"%.2f",_xPosition);
	sprintf(cube,"%.2f",_cube);
	sprintf(gap,"%.2f",_gap);
	sprintf(fontSize,"%.2f",_fontSize);
	sprintf(xName,"%.2f",_xName);
	sprintf(yName,"%.2f",_yName);
	sprintf(xValue,"%.2f", _xValue);
	sprintf(yValue,"%.2f", _yValue);
	sprintf(xSales,"%.2f", _xSales);
	sprintf(ySales,"%.2f", _ySales);
    sprintf(xMonths,"%.2f", _xMonths);
    sprintf(yMonths,"%.2f",_yMonths);

for(i = 0; i< cityCounter; i++)
{
	_yMonthss = i*(length)+(length * 110 / 400) ;
	sprintf(yMonthss,"%.2f", _yMonthss);
	for(j = 0; j < monthsCounter; j++)
	{
		_yPosition = _yPosition + _space;
		_yValue = _yValue + _space;
		sprintf(yPosition,"%.2f",_yPosition);
		sprintf(yValue,"%.2f",_yValue);
		sprintf(yMonthss,"%.2f", _yMonthss);
		
		children = xmlNewChild(rootNode, NULL, BAD_CAST "rect", NULL);
		xmlNewProp(children, BAD_CAST "x" , BAD_CAST xPosition);
		xmlNewProp(children, BAD_CAST "y" , BAD_CAST yMonthss);
		xmlNewProp(children, BAD_CAST "height" , BAD_CAST cube);
		xmlNewProp(children, BAD_CAST "width" , BAD_CAST cube);
		xmlNewProp(children, BAD_CAST "fill" , BAD_CAST colors[j]);
		_yPosition = _yPosition + (5 * length/300);
		sprintf(yPosition,"%.2f",_yPosition);

		children = xmlNewChild(rootNode, NULL,BAD_CAST "text", BAD_CAST xset.X_data[j]);
		xmlNewProp(children, BAD_CAST "x", BAD_CAST gap);
		xmlNewProp(children, BAD_CAST "y", BAD_CAST yMonthss);
		xmlNewProp(children, BAD_CAST "font-size", BAD_CAST fontSize);
		xmlNewProp(children, BAD_CAST "font-weight", BAD_CAST "bold");
		
		if(strcmp(ysetArray[i].showvalue,"yes")==0)
		{
		children = xmlNewChild(rootNode, NULL,BAD_CAST "text", BAD_CAST ysetArray[i].Y_data[j]);
		xmlNewProp(children, BAD_CAST "x", BAD_CAST xValue);
		xmlNewProp(children, BAD_CAST "y", BAD_CAST yMonthss);
		xmlNewProp(children, BAD_CAST "font-size", BAD_CAST fontSize);
		}
		_yMonthss = _yMonthss + _space;


	}
	_yMonthss = _yMonthss - _space;
	_yPosition = _yPosition + length * 150/300;
	_yValue = _yValue + length * 150/300;

}
	
	//printing city names
	for(i = 0; i< cityCounter; i++)
	{
		_fontSize = length * 18 / 300;
	    sprintf(fontSize,"%.2f",_fontSize);
		
		children = xmlNewChild(rootNode, NULL,BAD_CAST "text", BAD_CAST ysetArray[i].name);
		xmlNewProp(children, BAD_CAST "x", BAD_CAST xName);
		xmlNewProp(children, BAD_CAST "y", BAD_CAST yName);
		xmlNewProp(children, BAD_CAST "text-anchor", BAD_CAST "middle");
		xmlNewProp(children, BAD_CAST "font-size", BAD_CAST fontSize);
		xmlNewProp(children, BAD_CAST "font-weight", BAD_CAST "bold");
		xmlNewProp(children, BAD_CAST "fill" , BAD_CAST "#ff0000");

        _fontSize = length * 16 / 300;
	    sprintf(fontSize,"%.2f",_fontSize);
		
		children = xmlNewChild(rootNode, NULL,BAD_CAST "text", BAD_CAST axis.Y_name);
		xmlNewProp(children, BAD_CAST "x", BAD_CAST xSales);
		xmlNewProp(children, BAD_CAST "y", BAD_CAST ySales);
		xmlNewProp(children, BAD_CAST "font-size", BAD_CAST fontSize);

		children = xmlNewChild(rootNode, NULL,BAD_CAST "text", BAD_CAST axis.X_name);
		xmlNewProp(children, BAD_CAST "x", BAD_CAST xMonths);
		xmlNewProp(children, BAD_CAST "y", BAD_CAST yMonths);
		xmlNewProp(children, BAD_CAST "font-size", BAD_CAST fontSize);

		_yName = _yName + length;
		sprintf(yName,"%.2f",_yName);
		_ySales = _ySales + length;
		sprintf(ySales,"%.2f", _ySales);
		_yMonths = _yMonths + length;
		sprintf(yMonths,"%.2f",_yMonths);
	}
		
		//printing slices of the pie according to the input values
          x_center = length * 100 / 300;   
	      y_center = length * 150 / 300; 
		 _radius = length * 80 / 300; 
    
    for(i = 0; i < cityCounter; i++)
    {
	    for(j = 0; j < yDataCounter; j++)
	    {
	 
	    	if(j==0) //if first slice
	    	{
	    	   x_start = x_center - _radius;
	    	   y_start = y_center;
	    	}
	    	else
	    	{
	    		x_start = tempX;
	    		y_start = tempY;
	    	}
	    	if(atoi(ysetArray[i].Y_data[j]) > getTotal(i)/2) //if one slice is bigger than 180 degree
	    	{
	    		ifBiggerThanHalf=1;
	    	}
	    	else
	    	{
	    		ifBiggerThanHalf=0;
	    	}
	    	strChange = strChange + atoi(ysetArray[i].Y_data[j]);

		    rating =  PI * 2 * strChange / getTotal(i); //finding rating of the slice in order to draw it

		    x_result = x_center - (cos(rating) * _radius); //calculating x index of the finishing point of the slice
		    y_result = y_center - (sin(rating) * _radius); //calculating y index of the finishing point of the slice
		    tempX = x_result; //taking it as temp because finish point of a slice is starting point of the next one
		    tempY = y_result;

			char centerX[sizeof(x_center)],centerY[sizeof(y_center)] ,radius[sizeof(_radius)], xValue[sizeof(x_result)], yValue[sizeof(y_result)],xStart[sizeof(x_start)], yStart[sizeof(y_start)], ifBigger[sizeof(ifBiggerThanHalf)];
			sprintf(centerX,"%.2f",x_center);
			sprintf(centerY,"%.2f",y_center);
			sprintf(radius,"%.2f",_radius);
			sprintf(xValue,"%.2f",x_result);
			sprintf(yValue,"%.2f",y_result);
			sprintf(xStart,"%.2f",x_start);
			sprintf(yStart,"%.2f",y_start);
			sprintf(ifBigger,"%d",ifBiggerThanHalf);
            
            //result = "McenterX,centerY LxStart,yStart Aradius,radius 0 ifBigger,1 xValue,yValue Z"
			strcpy(result,M); //M
			strcat(result,centerX);
			strcat(result,vir);
			strcat(result,centerY);
			strcat(result,spacePie);
			strcat(result,L); //L
			strcat(result,xStart);
			strcat(result,vir);
			strcat(result,yStart);
			strcat(result,spacePie);
			strcat(result,A); //A
			strcat(result,radius);
			strcat(result,vir);
			strcat(result,radius);
			strcat(result,spacePie);
			strcat(result,"0");
			strcat(result,spacePie);
			strcat(result,ifBigger);
			strcat(result,vir);
			strcat(result,one);
			strcat(result,spacePie); //Z
			strcat(result,xValue);
			strcat(result,vir);
			strcat(result,yValue);
			strcat(result,spacePie);
			strcat(result,Z);
			strcpy(slice, "fill:");
			strcat(slice, colors[j]);
			strcat(slice, ";fill-opacity: 1;stroke:black;stroke-width: 0");

			children=xmlNewChild(rootNode,NULL,BAD_CAST"path",NULL);
			xmlNewProp(children,BAD_CAST"d",BAD_CAST result);
			xmlNewProp(children,BAD_CAST"style",BAD_CAST slice);
	    }

		y_center = y_center + length;
		tempX=0;
		tempY=0;
		strChange=0;
	}

	xmlSaveFormatFileEnc(outputFile, doc, "UTF-8", 1);
}

double getTotal(int ySet)
{
	double result = 0;
	int i;
	
	for(i = 0; i < yDataCounter; i++)
	{
		result = result + atoi(ysetArray[ySet].Y_data[i]);
	}

	return result;
}