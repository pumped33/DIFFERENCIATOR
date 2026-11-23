
#include <ctime>
#include <malloc.h>
#include <assert.h>
#include <sec_api/stdlib_s.h>
#include <string.h>
#include <time.h>
#include <direct.h>
#include <cstdint>
#include <stdlib.h>
#include <math.h>

#include "read_from_file.cpp"

#define SSCANF_IN_READ_INORDER(format, arg, len_parce_str_to_increase_buffer_IP)                                     \
                                sscanf(buffer + *current_buffer_IP, format, arg, len_parce_str_to_increase_buffer_IP); \
                                *current_buffer_IP += *len_parce_str_to_increase_buffer_IP;

#define PRINT_DUMP_NODE_WITH_CHARACTERISTICS(node_shape, node_fillcolor, first_block, second_block_specificator, second_block)      \
                fprintf(debug_out, "elem%zu [shape=" #node_shape", style=filled, margin=0.05, fillcolor=\"%s\","                     \
                                   "color = \"#585114\" label = \"%s\n" #second_block_specificator "\"];\n",                        \
                                  (size_t)node % 10000, node_fillcolor, first_block, second_block);    
 

#define GRAPHVIZ_MODE

#ifdef GRAPHVIZ_MODE
    #define ON_GRPV(...) __VA_ARGS__
#else
    #define ON_GRPV(...) 
#endif

const bool  LEFT_SIDE            = 0;
const bool  RIGHT_SIDE           = 1;

const size_t  MAXLEN_STR_INORDER     = 10;
const size_t  MAXLEN_OPERATION_ARRAY = 4;
const size_t  MAXLEN_VARIABLE_ARRAY  = 3;

//dump consts
const char MAXLEN_EXTENSION             = 4; // like .png  .htm
const char MAXLEN_FOLDER_NAME  = 75;
const int  MAXLEN_SYSTEM_STR            = MAXLEN_FOLDER_NAME +  50;
const char MAXLEN_STR_HTML_IMAGE_INSERT = 75;
const char FILE_PNG_GRAPHVIZ_NAME       = 75;
const char MAXLEN_HTML_DUMP_FILE_NAME   = 75;



enum var_t {

    VAR_FAIL = -1,
    VAR_X    = 0, 
    VAR_Y    = 1,
    VAR_Z    = 2, 
};

struct var_info_t {

    const char*    var_name;
    var_t          var_enum;
    double         var_value;
    const uint32_t hash;
};

enum op_t {
    
    OP_FAIL = 0,
    OP_ADD  = 1,
    OP_SUB  = 2, 
    OP_MUL  = 3, 
    OP_DIV  = 4, 
}; 

enum node_type_t {

    NODE_TYPE_FAIL = 0,
    NODE_TYPE_OP   = 1,
    NODE_TYPE_VAR  = 2, 
    NODE_TYPE_NUM  = 3, 

}; 


union data_t {

    double      konst;
    const char* variable;
    const char* operation;
};

struct TNode_t {

    node_type_t node_type = NODE_TYPE_FAIL;
    data_t  data          = {};

    TNode_t* left_node    = NULL;
    TNode_t* right_node   = NULL;
    TNode_t* parent_node  = NULL;
};

struct op_info_t {

    const char*   op_name                              = "";
    op_t          op_enum                              = OP_FAIL;
    double        (*operation_func)(double, double)    = NULL;
    TNode_t*      (*diff_func)(TNode_t* cur_node,
                               const char* diff_var, 
                               data_t* cur_data)      
                                                       = NULL;
    const uint32_t hash                                = 0;
};

struct parcer_t {

    char     symbol                      = 0; 
    char     cur_str[MAXLEN_STR_INORDER] = "";
    int      len_str                     = 0;

    TNode_t* cur_node                    = NULL;
    data_t   current_data                = {};
    TNode_t* current_left_node           = NULL;

    double current_konst                 = 0;
    
    size_t op_index_arr                  = 0;
    size_t var_index_arr                 = 0;
};

struct bsearch_t {

    ssize_t left        = 0;
    ssize_t right       = 0;
    ssize_t mid         = 0;
    size_t  len_command = 0; 
};

struct graph_dump_t {
    
    int number_of_dumps                                    = 1;

    FILE* fp_html                                          = NULL;
    char  folder_name[MAXLEN_FOLDER_NAME]         = "";
    char  html_file_dump_name[MAXLEN_HTML_DUMP_FILE_NAME]  = "";
};

//create node
void     create_links_by_parent            (TNode_t* new_node_by_parents, TNode_t* parent, const bool parent_side);
TNode_t* new_node_by_parents               (TNode_t* parent, const bool parent_side, node_type_t node_type, const data_t data_value);
//read tree
TNode_t* tree_read_inorder                 (TNode_t* parent, const bool parent_side, char* buffer, size_t* current_buffer_IP);
TNode_t* tree_read_from_file               (const char* filename);
void     read_qwote_string                 (TNode_t* parent, const bool parent_side, parcer_t* parcer_info, char* buffer, size_t* current_buffer_IP);
void     read_number                       (TNode_t* parent, const bool parent_side, parcer_t* parcer_info, char* buffer, size_t* current_buffer_IP);
//print node
void     print_tree_inorder_in_file        (const TNode_t* cur_node, FILE* fp_out);

// hash func
uint32_t djb_2_hash                        (const char* str, unsigned int len);
//
ssize_t  find_operation                    (const char* current_command);
ssize_t  find_variable                     (const char* current_var);

//comporators
int      comporator_quick_sort_hash_up_var (const void* arg1, const void* arg2);
int      comporator_quick_sort_hash_up_op  (const void* arg1, const void* arg2);
int      comporator_quick_sort_enum_up_op  (const void* arg1, const void* arg2);


//graphviz dump
void     print_links_in_dump               (FILE* debug_out, const TNode_t* node, const char* color_right, const char* color_failure);
void     create_element_dump               (const TNode_t* node, const TNode_t* last_add_node, FILE* debug_out);
void     fprintf_html_head                 (graph_dump_t dump_info);
void     fprintf_html_capture              (graph_dump_t dump_info);
void     fprintf_html_border               (graph_dump_t dump_info);
void     tree_dump_html                    (const TNode_t* main_node, const TNode_t* last_add_node);
void     tree_graph_dump                   (const TNode_t* main_node, const TNode_t* last_add_node, char* name_graphviz_file_out, graph_dump_t dump_info);

//diff and calculate
double   count_tree_expression             (TNode_t* cur_node);
TNode_t* differenciator                    (TNode_t* cur_node, const char* diff_var);

//operation funcs
double   sum                               (double x1, double x2);
double   mul                               (double x1, double x2);
double   sub                               (double x1, double x2);
double   div                               (double x1, double x2);

//diff func
TNode_t* diff_sum                          (TNode_t* cur_node, const char* diff_var, data_t* cur_data);
TNode_t* diff_sub                          (TNode_t* cur_node, const char* diff_var, data_t* cur_data);
TNode_t* diff_mul                          (TNode_t* cur_node, const char* diff_var, data_t* cur_data);
TNode_t* diff_div                          (TNode_t* cur_node, const char* diff_var, data_t* cur_data);

//latech dump
void     add_latex_formula                 (TNode_t* cur_node, FILE* fp_latech);

op_info_t OPERATION_ARRAY[MAXLEN_OPERATION_ARRAY] = {
    {"+", OP_ADD, sum, diff_sum, djb_2_hash("+", 1)}, 
    {"*", OP_MUL, mul, diff_mul, djb_2_hash("*", 1)},
    {"-", OP_SUB, sub, diff_sub, djb_2_hash("-", 1)},
    {"/", OP_DIV, div, diff_div, djb_2_hash("/", 1)}
};

var_info_t VARIABLE_ARRAY[MAXLEN_VARIABLE_ARRAY] = {
    {"x", VAR_X, 1, djb_2_hash("x", 1)},
    {"y", VAR_Y, 2, djb_2_hash("y", 1)},
    {"z", VAR_Z, 3, djb_2_hash("z", 1)}
};

int main() {

    FILE* fp = fopen("diff_out.txt", "w");
    TNode_t* main_node = tree_read_from_file("C:\\Users\\alexk\\OneDrive\\Desktop\\IT_MIPT_2026\\DIFFERENCIATOR\\inorder.txt");

    // data_t main_data;
    // main_data.operation = "+";
    // TNode_t* main_node = new_node_by_parents(NULL, LEFT_SIDE, NODE_TYPE_OP, main_data);

    // data_t l1;
    // l1.konst = 6;
    // TNode_t* l1_node = new_node_by_parents(main_node, LEFT_SIDE, NODE_TYPE_NUM, l1);

    // data_t r1;
    // r1.variable = "x";
    // TNode_t* r1_node = new_node_by_parents(main_node, RIGHT_SIDE, NODE_TYPE_VAR, r1);
    print_tree_inorder_in_file(main_node, fp);
    TNode_t* diff_node = differenciator(main_node, "y");
    printf("D - %lf\n", diff_node -> data.konst);
    printf("E - %lf\n", count_tree_expression(main_node));
    tree_dump_html(main_node, NULL);
    return 0;
}

TNode_t* new_node_by_parents(TNode_t* parent, const bool parent_side, node_type_t node_type, const data_t data_value) {

    TNode_t* new_node = (TNode_t*)calloc(1, sizeof(TNode_t)); 
    new_node -> node_type = node_type;
    new_node-> data = data_value;
    
    create_links_by_parent(new_node, parent, parent_side);

    return new_node;
}

void create_links_by_parent(TNode_t* new_node, TNode_t* parent, const bool parent_side) {

    assert(new_node);
    new_node -> parent_node = parent;

    if (parent) {
        if (parent_side == LEFT_SIDE) parent -> left_node  = new_node;
        else                          parent -> right_node = new_node; 
    }
}

// TNode_t* new_node_by_children(TNode_t* left_child, TNode_t* right_child, node_type_t node_type, const data_t data_value) {

//     TNode_t* new_node = (TNode_t*)calloc(1, sizeof(TNode_t)); 
//     new_node -> node_type = node_type;
//     new_node -> data = data_value;
    
//     create_links_by_parent(new_node, left_child, right_child);

//     return new_node;
// }

// void create_links_by_children(TNode_t* new_node, TNode_t* left_child, TNode_t* right_child) {

//     assert(new_node_by_parents);
//     new_node -> left_node  = right_child;
//     new_node -> left_node  = left_child;

//     if (left_child) new_node ->  left_node -> parent_node = new_node; 
//     if (left_child) new_node -> right_node -> parent_node = new_node; 
    
// }


void print_tree_inorder_in_file(const TNode_t* cur_node, FILE* fp_out) {

    assert(cur_node);
    assert(fp_out);

    fprintf(fp_out, "( ");

    if (cur_node -> left_node) print_tree_inorder_in_file(cur_node -> left_node, fp_out);
    else                       fprintf(fp_out, "nil ");
    
    switch(cur_node -> node_type) {
        case NODE_TYPE_OP:   fprintf(fp_out, "\"%s\" ", cur_node -> data.operation);
                             break;
 
        case NODE_TYPE_VAR:  fprintf(fp_out, "\"%s\" ", cur_node -> data.variable);
                             break;
        case NODE_TYPE_NUM:  fprintf(fp_out, "%lf ", cur_node -> data.konst);
                             break;

        default:             fprintf(fp_out, "i don't know this command");
                             break;
    }

    if (cur_node -> right_node) print_tree_inorder_in_file(cur_node -> right_node, fp_out);
    else                        fprintf(fp_out, "nil ");

    fprintf(fp_out, ") "); 
    // ON_GRPV(tree_dump_html(main_node, last_add_node)); // Hueta 
}
TNode_t* tree_read_from_file(const char* filename) {

    assert(filename);

    file_info_t file_info = {};
    char* buffer = read_from_file(&file_info, filename);

    size_t current_buffer_IP = 0;

    qsort(::OPERATION_ARRAY,::MAXLEN_OPERATION_ARRAY, sizeof(::OPERATION_ARRAY[0]), comporator_quick_sort_hash_up_op);
    qsort(::VARIABLE_ARRAY, ::MAXLEN_VARIABLE_ARRAY,  sizeof(::VARIABLE_ARRAY[0]), comporator_quick_sort_hash_up_var);

    TNode_t* main_node =  tree_read_inorder(NULL, LEFT_SIDE, buffer, &current_buffer_IP);

    ON_GRPV(tree_dump_html(main_node, NULL));

    free(buffer);
    return main_node;
}


TNode_t* tree_read_inorder(TNode_t* parent, const bool parent_side, char* buffer, size_t* current_buffer_IP) {

    parcer_t parcer_info = {};

    SSCANF_IN_READ_INORDER(" %c%n", &parcer_info.symbol, &parcer_info.len_str)

    if (parcer_info.symbol == 'n') {
        SSCANF_IN_READ_INORDER("%2s%n",  parcer_info.cur_str, &parcer_info.len_str)
        if (strcmp(parcer_info.cur_str, "il") == 0)  return NULL;
    }

    if (parcer_info.symbol == '(') {
       parcer_info.current_left_node = tree_read_inorder(NULL, LEFT_SIDE, buffer, current_buffer_IP);
       SSCANF_IN_READ_INORDER(" %c%n", &parcer_info.symbol, &parcer_info.len_str);
    }

    
    if (parcer_info.symbol == '"') read_qwote_string(parent, parent_side, &parcer_info, buffer, current_buffer_IP);
    else                           read_number      (parent, parent_side, &parcer_info, buffer, current_buffer_IP);

    if (parcer_info.cur_node != NULL)  parcer_info.cur_node -> left_node = parcer_info.current_left_node;
    if (parcer_info.current_left_node) parcer_info.current_left_node -> parent_node = parcer_info.cur_node;

    tree_read_inorder(parcer_info.cur_node, RIGHT_SIDE, buffer, current_buffer_IP);

    SSCANF_IN_READ_INORDER(" %c%n", &parcer_info.symbol, &parcer_info.len_str)
    if (parcer_info.symbol == ')') return parcer_info.cur_node;

    return NULL;
}

void read_qwote_string(TNode_t* parent, const bool parent_side, parcer_t* parcer_info, char* buffer, size_t* current_buffer_IP) { 

        SSCANF_IN_READ_INORDER("%11[^\"]\"%n",  parcer_info -> cur_str, &parcer_info -> len_str)

        parcer_info -> op_index_arr  = find_operation(parcer_info -> cur_str);
        parcer_info -> var_index_arr = find_variable(parcer_info -> cur_str);
        printf("%s\n", parcer_info -> cur_str);
        if (parcer_info -> op_index_arr  != -1) {
             parcer_info -> current_data.operation = OPERATION_ARRAY[parcer_info -> op_index_arr].op_name;
             parcer_info -> cur_node = new_node_by_parents(parent, parent_side, NODE_TYPE_OP, parcer_info -> current_data);
        }

        else if (parcer_info -> var_index_arr != -1) {
             parcer_info -> current_data.variable = VARIABLE_ARRAY[parcer_info -> var_index_arr].var_name;
             parcer_info -> cur_node = new_node_by_parents(parent, parent_side, NODE_TYPE_VAR, parcer_info -> current_data);
        }
        else printf("I don't know this operation or variable\n");
}

void read_number(TNode_t* parent, const bool parent_side, parcer_t* parcer_info, char* buffer, size_t* current_buffer_IP) {

        (*current_buffer_IP)--;
        SSCANF_IN_READ_INORDER(" %lg%n", &parcer_info -> current_konst, &parcer_info -> len_str)
        parcer_info -> current_data.konst = parcer_info -> current_konst;
        parcer_info -> cur_node = new_node_by_parents(parent, parent_side, NODE_TYPE_NUM, parcer_info -> current_data);
}


uint32_t djb_2_hash(const char* str, unsigned int len) {
    
    uint32_t hash = 5381;

    for (unsigned int i = 0; i < len; i++) {
        hash = ((hash << 5) + hash) + *(str + i);
    }
    return hash; 
}

ssize_t find_operation(const char* current_command) {

    bsearch_t bsearch_info = {};
    bsearch_info.right = MAXLEN_OPERATION_ARRAY - 1;
    bsearch_info.len_command = strlen(current_command);

    while(bsearch_info.left <= bsearch_info.right) { //equal to bsearch()

        bsearch_info.mid = (bsearch_info.left + bsearch_info.right) / 2;
        int32_t res = ((int32_t)OPERATION_ARRAY[bsearch_info.mid].hash - (int32_t)djb_2_hash(current_command, bsearch_info.len_command));

        if (res == 0) {
            return bsearch_info.mid;
        }        
        else if (res > 0) {
            bsearch_info.right = bsearch_info.mid - 1;
        }
        else {
            bsearch_info.left  = bsearch_info.mid + 1;
        }  
    }
    return -1;
}


ssize_t find_variable(const char* current_var) {

    bsearch_t bsearch_info = {};
    bsearch_info.right = MAXLEN_VARIABLE_ARRAY - 1;
    bsearch_info.len_command = strlen(current_var);

    while(bsearch_info.left <= bsearch_info.right) { //equal to bsearch()

        bsearch_info.mid  = (bsearch_info.left + bsearch_info.right) / 2;
        int res = ((int32_t)VARIABLE_ARRAY[bsearch_info.mid].hash - (int32_t)djb_2_hash(current_var, bsearch_info.len_command));

        if (res == 0) {
            return bsearch_info.mid;
        }        
        else if(res > 0) {
            bsearch_info.right = bsearch_info.mid - 1;
        }
        else {
            bsearch_info.left  = bsearch_info.mid + 1;
        }  
    }
    return -1;
}

int comporator_quick_sort_hash_up_op(const void* arg1, const void* arg2) { 

    op_info_t* first_arg =  (op_info_t*) arg1;
    op_info_t* second_arg = (op_info_t*) arg2;

    if (first_arg -> hash < second_arg -> hash) {
        return -1;
    }
    return (int)(first_arg -> hash - second_arg -> hash);
}

int comporator_quick_sort_hash_up_var(const void* arg1, const void* arg2) { 

    var_info_t* first_arg =  (var_info_t*) arg1;
    var_info_t* second_arg = (var_info_t*) arg2;

    if (first_arg -> hash < second_arg -> hash) {
        return -1;
    }
    return (int)(first_arg -> hash - second_arg -> hash);
}

#ifdef GRAPHVIZ_MODE

    void tree_graph_dump(const TNode_t* main_node, const TNode_t* last_add_node, char* name_graphviz_file_out, graph_dump_t dump_info) { 

        snprintf(name_graphviz_file_out, FILE_PNG_GRAPHVIZ_NAME, "%s/log_%d.dot", dump_info.folder_name, dump_info.number_of_dumps);
        FILE* debug_out = fopen(name_graphviz_file_out, "w");

        //general settings
        fprintf(debug_out, "digraph List {\n"
                            "rankdir = TB;    \n"
                            "ranksep = 0.7    \n"
                            "nodesep = 0.7;   \n"
                            "splines = true;\n");
            
        create_element_dump(main_node, last_add_node, debug_out);
        fprintf(debug_out, "}\n");

        fclose(debug_out);

        char system_str[MAXLEN_FOLDER_NAME + 50] = "";
        snprintf(system_str, MAXLEN_SYSTEM_STR,  "dot -Tpng \"%s\" -o \"%.*s.png\"", name_graphviz_file_out, 
                (int)strlen(name_graphviz_file_out) - MAXLEN_EXTENSION, name_graphviz_file_out);
        system(system_str);

        char* extension = strchr(name_graphviz_file_out, '.');
        strcpy(extension, ".png");
    }

    void tree_dump_html(const TNode_t* main_node, const TNode_t* last_add_node) {

        static graph_dump_t dump_info = {};
        if (dump_info.number_of_dumps == 1) {

            time_t now = time(NULL);
            tm* t      = localtime(&now);
            strftime(dump_info.folder_name, MAXLEN_FOLDER_NAME, "logs_images_%m-%d_%H-%M-%S", t);
            _mkdir(dump_info.folder_name);

            snprintf(dump_info.html_file_dump_name, MAXLEN_HTML_DUMP_FILE_NAME, "%s/general_dump.html", dump_info.folder_name);
            dump_info.fp_html = fopen(dump_info.html_file_dump_name, "w");
            fprintf_html_head(dump_info);
            
            fclose(dump_info.fp_html);
        }

        dump_info.fp_html = fopen(dump_info.html_file_dump_name, "a");
        fprintf_html_border(dump_info);
        fprintf_html_capture(dump_info);

        // Generate PNG
        char name_graphviz_file[FILE_PNG_GRAPHVIZ_NAME];
        tree_graph_dump(main_node, last_add_node, name_graphviz_file, dump_info);


        // Two blocks: dump and image
        fprintf(dump_info.fp_html,
            "<div style=\"display: flex; flex-direction: row; "
                    "gap: 40px; align-items: flex-start; margin-bottom: 30px;\">\n"
        );
        // IMAGE
        char image_html_str[MAXLEN_STR_HTML_IMAGE_INSERT] = "";
        char* image_name = strrchr(name_graphviz_file, '/') + 1;
        snprintf(image_html_str, MAXLEN_STR_HTML_IMAGE_INSERT, "<img src=\"%s\" width=\"1250\">\n", image_name);
        fprintf(dump_info.fp_html, "%s", image_html_str);

        // HTML ending
        fprintf(dump_info.fp_html,"</div>\n");
        fprintf_html_border(dump_info);

        fclose(dump_info.fp_html);
        dump_info.number_of_dumps++; 
    }

    void create_element_dump(const TNode_t* node, const TNode_t* last_add_node, FILE* debug_out) {

        switch(node -> node_type) {
            case NODE_TYPE_OP:  PRINT_DUMP_NODE_WITH_CHARACTERISTICS(circle, "#F4A261", "OP", %s, node -> data.operation)
                                break;

            case NODE_TYPE_VAR: PRINT_DUMP_NODE_WITH_CHARACTERISTICS(diamond, "#64B5F6", "VAR", %s, node -> data.variable)
                                break;

            case NODE_TYPE_NUM: PRINT_DUMP_NODE_WITH_CHARACTERISTICS(box,     "#81C784", "NUM", %lg, node -> data.konst)
                                break;

            default:            printf("Unknown operatoin");
                                break;
        };          

        if (node -> left_node)             create_element_dump(node -> left_node,  last_add_node, debug_out);
        if (node -> right_node)            create_element_dump(node -> right_node, last_add_node, debug_out);

        if (node -> left_node && node -> right_node) {
            fprintf(debug_out, "{rank = same; elem%zu; elem%zu};", 
            (size_t)node -> left_node % 10000, (size_t)node -> right_node % 10000);
        }

        fprintf(debug_out, "edge[dir=\"forward\", weight=0, arrowsize=1];");
        print_links_in_dump(debug_out, node -> left_node, "#133c0c",  "#f30e0e");
        print_links_in_dump(debug_out, node -> right_node,"#133c0c",  "#f30e0e");
    }

    void print_links_in_dump(FILE* debug_out, const TNode_t* node, const char* color_right, const char* color_failure) {
        
        if (node) {
            if (node  == node ->  parent_node -> left_node || node == node ->  parent_node -> right_node)
                fprintf(debug_out, "elem%zu -> elem%zu[color = \"%s\"];\n", 
                        (size_t)node -> parent_node % 10000, (size_t)node % 10000, color_right);
            else 
                fprintf(debug_out, "elem%zu -> elem%zu[color = \"%s\"];\n", 
                        (size_t)node -> parent_node % 10000, (size_t)node % 10000, color_failure);
        }
    }

    void fprintf_html_border(graph_dump_t dump_info) {
        fprintf(dump_info.fp_html,      "<hr style=\""
                                        "border: none;"
                                        "height: 5px;"
                                        "background-color: #34495e;"
                                        "\">\n");
    }

    void fprintf_html_capture(graph_dump_t dump_info) {
        fprintf(dump_info.fp_html, "<h2 style = \"font-family: Georgia, serif;" 
                                                    "font-size: 42px;"
                                                    "color: #34495e;\">"
                                                    "Dump #%d</h2>\n", dump_info.number_of_dumps);
    }

    void fprintf_html_head(graph_dump_t dump_info) {
    fprintf(dump_info.fp_html,
                "<!DOCTYPE html>\n"
                        "<html>\n\n"
                        "<head>\n"
                        "\t<meta charset=\"UTF-8\">\n"
                        "\t<title>List dump</title>\n"
                        "</head>\n\n"
                        "<body>\n");
    }

#endif

double count_tree_expression(TNode_t* cur_node) {

    assert(cur_node);

    if (cur_node -> parent_node == NULL) qsort(::VARIABLE_ARRAY, ::MAXLEN_VARIABLE_ARRAY,  
                                     sizeof(::VARIABLE_ARRAY[0]), comporator_quick_sort_hash_up_var);
    
    if      (cur_node -> node_type == NODE_TYPE_NUM) return cur_node -> data.konst;
    else if (cur_node -> node_type == NODE_TYPE_VAR) return ::VARIABLE_ARRAY[find_variable(cur_node -> data.variable)].var_value;   
    else {
        double left_data  = count_tree_expression(cur_node -> left_node);
        double right_data = count_tree_expression(cur_node -> right_node);

        ssize_t index_op_arr = find_operation((cur_node -> data).operation);
        assert(index_op_arr != -1);
        double (*operation_func)(double, double) = ::OPERATION_ARRAY[index_op_arr].operation_func;
    
        return operation_func(left_data, right_data);
    }
}

double sum(double x1, double x2) {
    return x1 + x2;
}
double mul(double x1, double x2) {
    return x1 * x2;
}
double sub(double x1, double x2) {
    return x1 - x2;
}

double div(double x1, double x2) {

    if (x2 == 0) return 0;

    return x1 / x2;
}

TNode_t* differenciator(TNode_t* cur_node, const char* diff_var) {

    assert(cur_node);

    if (cur_node -> parent_node == NULL) qsort(::VARIABLE_ARRAY, ::MAXLEN_VARIABLE_ARRAY,  
                                     sizeof(::VARIABLE_ARRAY[0]), comporator_quick_sort_hash_up_var);

    data_t cur_data = {};
    
    if (cur_node -> node_type == NODE_TYPE_NUM) {
        cur_data.konst = 0;
        return new_node_by_parents(NULL, LEFT_SIDE, NODE_TYPE_NUM, cur_data);
    }

    else if (cur_node -> node_type == NODE_TYPE_VAR) {
        if (strcmp(cur_node -> data.variable, diff_var) == 0) cur_data.konst = 1;
        else                                                             cur_data.konst = 0;

        return new_node_by_parents(NULL, LEFT_SIDE, NODE_TYPE_NUM, cur_data);
    }

    else {
        size_t index_op_arr = find_operation((cur_node -> data).operation);
        TNode_t* (*cur_diff_func)(TNode_t* cur_node, const char* diff_var, data_t* cur_data) = ::OPERATION_ARRAY[index_op_arr].diff_func;
        return cur_diff_func(cur_node, diff_var, &cur_data);
    }
}

TNode_t* diff_sum(TNode_t* cur_node, const char* diff_var, data_t* cur_data) {

    cur_data -> konst =  differenciator(cur_node -> left_node, diff_var) -> data.konst + differenciator(cur_node -> right_node, diff_var) -> data.konst;
    return new_node_by_parents(NULL, LEFT_SIDE, NODE_TYPE_NUM, *cur_data);
}

TNode_t* diff_sub(TNode_t* cur_node, const char* diff_var, data_t* cur_data) {

    cur_data -> konst =  differenciator(cur_node -> left_node, diff_var) -> data.konst - differenciator(cur_node -> right_node, diff_var) -> data.konst;
    return new_node_by_parents(NULL, LEFT_SIDE, NODE_TYPE_NUM, *cur_data);
}

TNode_t* diff_mul(TNode_t* cur_node, const char* diff_var, data_t* cur_data) {

    cur_data -> konst = differenciator(cur_node -> left_node, diff_var) -> data.konst * count_tree_expression(cur_node -> right_node) +
                        differenciator(cur_node -> right_node, diff_var) -> data.konst * count_tree_expression(cur_node -> left_node);

    return new_node_by_parents(NULL, LEFT_SIDE, NODE_TYPE_NUM, *cur_data);
}

TNode_t* diff_div(TNode_t* cur_node, const char* diff_var, data_t* cur_data) {

    cur_data -> konst = (differenciator(cur_node -> left_node, diff_var) -> data.konst * count_tree_expression(cur_node -> right_node) +
                        differenciator(cur_node -> right_node, diff_var) -> data.konst * count_tree_expression(cur_node -> left_node)) / 
                        pow(count_tree_expression(cur_node -> right_node), 2);

    return new_node_by_parents(NULL, LEFT_SIDE, NODE_TYPE_NUM, *cur_data);
}

void latex_dump(TNode_t* cur_node) {

    static char folder_name[MAXLEN_FOLDER_NAME] = "";
    static char latech_file_name[MAXLEN_FOLDER_NAME] = "";
    static unsigned int number_of_dumps         = 1;
    static FILE* fp_latech                      = NULL;

    if (number_of_dumps == 1) {

        time_t now = time(NULL);
        tm* t      = localtime(&now);
        strftime(folder_name, MAXLEN_FOLDER_NAME, "LATECH_%m-%d_%H-%M-%S", t);
        _mkdir(folder_name);

    }
    snprintf(latech_file_name, FILE_PNG_GRAPHVIZ_NAME, "%s/tech_%d.tex", folder_name, number_of_dumps);
    fp_latech = fopen(latech_file_name, "w");
    fprintf(fp_latech,"\\documentclass{article} \n\n"
                             "\\title{Dump №%u}\n", number_of_dumps);

    fprintf(fp_latech, "\\begin{document}\n\n");
    fprintf(fp_latech, "$");
    add_latex_formula(cur_node, fp_latech);
    fprintf(fp_latech, "$\n\n");
    fprintf(fp_latech, "\\end{document}");
}

void add_latex_formula(TNode_t* cur_node, FILE* fp_latech) {

    if (cur_node -> parent_node == NULL) {
         qsort(OPERATION_ARRAY, MAXLEN_OPERATION_ARRAY, sizeof(OPERATION_ARRAY[0]), comporator_quick_sort_hash_up_op);
    }

    if (cur_node -> node_type == NODE_TYPE_OP) {
        fprintf(fp_latech, "(");
        op_t cur_node_op_enum = ::OPERATION_ARRAY[find_operation(cur_node -> data.operation)].op_enum;
        
        if (cur_node_op_enum == OP_DIV) {
            fprintf(fp_latech, "\\frac{");
            add_latex_formula(cur_node -> left_node, fp_latech);
            fprintf(fp_latech, "}{");
            add_latex_formula(cur_node -> right_node, fp_latech);
            fprintf(fp_latech, "})");
            return;
        }

        add_latex_formula(cur_node -> left_node, fp_latech);

        switch(cur_node_op_enum) {
            case OP_ADD: fprintf(fp_latech, "+");
                         break;

            case OP_SUB: fprintf(fp_latech, "-");
                         break; 

            case OP_MUL: fprintf(fp_latech, "\\cdot");
                         break; 

            default:     break;
        };
        add_latex_formula(cur_node -> right_node, fp_latech);
        fprintf(fp_latech, ")");
    }

    else if (cur_node -> node_type == NODE_TYPE_VAR) {
        ssize_t cur_index_var = find_variable(cur_node -> data.variable);
        fprintf(fp_latech, "%s", ::VARIABLE_ARRAY[cur_index_var].var_name);
        return;
    }
    else {
        fprintf(fp_latech, "%lf", cur_node -> data.konst);
        return;
    }     
}