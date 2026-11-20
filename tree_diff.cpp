

#include <malloc.h>
#include <assert.h>
#include <string.h>
#include <time.h>
#include <direct.h>
#include <cstdint>

#include "read_from_file.cpp"


#define SSCANF_IN_READ_PREORDER(format, arg, len_parce_str_to_increase_buffer_IP)                                     \
                                sscanf(buffer + *current_buffer_IP, format, arg, len_parce_str_to_increase_buffer_IP); \
                                *current_buffer_IP += *len_parce_str_to_increase_buffer_IP;



const bool  LEFT_SIDE            = 0;
const bool  RIGHT_SIDE           = 1;

const int   MAXLEN_STR_INORDER  = 10;
const int   MAXLEN_OPERATION_ARRAY = 4;
const int MAXLEN_VARIABLE_ARRAY  = 3;



//dump consts
const int MAXLEN_EXTENSION             = 4; // like .png  .htm
const int MAXLEN_GRAPHVIZ_FOLDER_NAME  = 75;
const int MAXLEN_SYSTEM_STR            = MAXLEN_GRAPHVIZ_FOLDER_NAME +  50;
const int MAXLEN_STR_HTML_IMAGE_INSERT = 75;
const int FILE_PNG_GRAPHVIZ_NAME       = 75;
const int MAXLEN_HTML_DUMP_FILE_NAME   = 75;

enum var_t {

    VAR_FAIL = -1,
    VAR_X = 0, 
    VAR_Y = 1,
    VAR_Z = 2, 
};

struct var_info_t {

    const char*    var_name;
    var_t          var_enum;
    const uint32_t hash_command;
};

enum op_t {
    
    OP_FAIL = 0,
    OP_ADD  = 1,
    OP_SUB  = 2, 
    OP_MUL  = 3, 
    OP_DIV  = 4, 
}; 

struct op_info_t {

    const char*    op_name;
    op_t         node_type_enum;
    const uint32_t hash_command;
};

enum node_type_t {

    NODE_TYPE_FAIL = 0,
    NODE_TYPE_OP = 1,
    NODE_TYPE_VAR = 2, 
    NODE_TYPE_NUM  = 3, 

}; 


union data_t {

    double      konst;
    const char* variable;
    const char* operation;
};

struct TNode_t {

    node_type_t node_type = NODE_TYPE_FAIL;

    data_t   data;

    TNode_t* left_node   = NULL;
    TNode_t* right_node  = NULL;

    TNode_t* parent_node = NULL;
};

struct parcer_t {

    char     symbol                  = 0; 
    char     cur_str[MAXLEN_STR_INORDER] = "";
    int      len_str                 = 0;

    TNode_t* cur_node                = NULL;

};

struct bsearch_t {

    int left  = 0;
    int right = 0;
    int mid   = 0;
    int len_command = 0; 
};


void     create_links(TNode_t* new_node, TNode_t* parent, const bool parent_side);
TNode_t* new_node(TNode_t* parent, const bool parent_side, node_type_t node_type, const data_t data_value);
void     print_tree_inorder_in_file(const TNode_t* cur_node, FILE* fp_out);

TNode_t* tree_read_inorder(TNode_t* parent, const bool parent_side, char* buffer, size_t* current_buffer_IP);
TNode_t* tree_read_from_file(const char* filename);

uint32_t djb_2_hash(const char* str, unsigned int len);
ssize_t find_operation(const char* current_command);

ssize_t find_variable(const char* current_var);

op_info_t OPERATION_ARRAY[MAXLEN_OPERATION_ARRAY] = {
    {"ADD", OP_ADD, djb_2_hash("+", 1)}, 
    {"MUL", OP_MUL, djb_2_hash("*", 1)},
    {"SUB", OP_SUB, djb_2_hash("-", 1)},
    {"DIV", OP_DIV, djb_2_hash("\\", 1)}
};

var_info_t VARIABLE_ARRAY[MAXLEN_VARIABLE_ARRAY] = {
    {"x", VAR_X, djb_2_hash("x", 1)},
    {"y", VAR_Y, djb_2_hash("y", 1)},
    {"z", VAR_Z, djb_2_hash("z", 1)}
};

                                
                                

int main() {
    FILE* fp = fopen("diff_out.txt", "w");
    TNode_t* main_node = tree_read_from_file("C:\\Users\\alexk\\OneDrive\\Desktop\\IT_MIPT_2026\\DIFFERENCIATOR\\inorder.txt");

    // data_t main_data;
    // main_data.operation = "+";
    // TNode_t* main_node = new_node(NULL, LEFT_SIDE, NODE_TYPE_OP, main_data);

    // data_t l1;
    // l1.konst = 6;
    // TNode_t* l1_node = new_node(main_node, LEFT_SIDE, NODE_TYPE_NUM, l1);

    // data_t r1;
    // r1.konst = 7;
    // TNode_t* r1_node = new_node(main_node, RIGHT_SIDE, NODE_TYPE_NUM, r1);


    print_tree_inorder_in_file(main_node, fp);

    return 0;

}

TNode_t* new_node(TNode_t* parent, const bool parent_side, node_type_t node_type, const data_t data_value) {

    TNode_t* new_node = (TNode_t*)calloc(1, sizeof(TNode_t)); 
    new_node -> node_type = node_type;
    new_node -> data = data_value;
    
    create_links(new_node, parent, parent_side);

    return new_node;
}

void create_links(TNode_t* new_node, TNode_t* parent, const bool parent_side) {

    assert(new_node);
    new_node -> parent_node = parent;

    if (parent) 
        if (parent_side == LEFT_SIDE) parent -> left_node  = new_node;
        else                          parent -> right_node = new_node; 
}


void print_tree_inorder_in_file(const TNode_t* cur_node, FILE* fp_out) {

    assert(cur_node);
    assert(fp_out);

    fprintf(fp_out, "( ");

    if (cur_node -> left_node)       print_tree_inorder_in_file(cur_node -> left_node, fp_out);
    else                             fprintf(fp_out, "nil ");
    
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


    if (cur_node -> right_node)     print_tree_inorder_in_file(cur_node -> right_node, fp_out);
    else                            fprintf(fp_out, "nil ");

    fprintf(fp_out, ") "); 
    

    // ON_GRPV(list_dump_html(main_node, last_add_node)); // Hueta 
}
TNode_t* tree_read_from_file(const char* filename) {

    assert(filename);

    file_info_t file_info = {};
    char* buffer = read_from_file(&file_info, filename);
    assert(file_info.ptr_file_in);


    size_t current_buffer_IP         = 0;


    TNode_t* main_node =  tree_read_inorder(NULL, LEFT_SIDE, buffer, &current_buffer_IP);

    free(buffer);
    return main_node;
    // ON_GRPV(list_dump_html(main_node, last_add_node)); // Hueta 
}

TNode_t* tree_read_inorder(TNode_t* parent, const bool parent_side, char* buffer, size_t* current_buffer_IP) {

    parcer_t parcer_info = {};

    SSCANF_IN_READ_PREORDER(" %c%n", &parcer_info.symbol, &parcer_info.len_str)

    printf("[1c = %c]\n", parcer_info.symbol);
    getchar();

    if (parcer_info.symbol == 'n') {
        SSCANF_IN_READ_PREORDER("%2s%n",  parcer_info.cur_str, &parcer_info.len_str)
        printf("[str in nil = %s]\n", parcer_info.cur_str);
        getchar();
        if (strcmp(parcer_info.cur_str, "il") == 0) { 
            return NULL;
        }
    }

    data_t current_data = {};

    if (parcer_info.symbol == '(') {
        tree_read_inorder(parcer_info.cur_node, LEFT_SIDE, buffer, current_buffer_IP);
    }
    else if (parcer_info.symbol == '"') {
        const ssize_t index_op_arr =  find_operation(parcer_info.cur_str);
        const ssize_t index_variable_arr = find_variable(parcer_info.cur_str);

        if (index_op_arr != -1) {
            current_data.operation = OPERATION_ARRAY[index_op_arr].op_name;
            parcer_info.cur_node = new_node(parent, parent_side, NODE_TYPE_OP, current_data);
        }
        if (index_variable_arr != -1) {
            current_data.variable = VARIABLE_ARRAY[index_variable_arr].var_name;
        }
        (*current_buffer_IP)++;
    }
    else {
        (*current_buffer_IP)--;
        double current_konst =  0;
        SSCANF_IN_READ_PREORDER("%lg%n", &current_konst, &parcer_info.len_str)
        printf("[double  = %lf]\n", current_konst);
        getchar();
        current_data.konst = current_konst;
        parcer_info.cur_node = new_node(parent, parent_side, NODE_TYPE_NUM, current_data);
    }

    tree_read_inorder(parcer_info.cur_node, RIGHT_SIDE, buffer, current_buffer_IP);
    SSCANF_IN_READ_PREORDER(" %c%n", &parcer_info.symbol, &parcer_info.len_str)
    printf("[2c = %c]\n", parcer_info.symbol);
    getchar();
    if (parcer_info.symbol == ')') {
        return parcer_info.cur_node;
    }

    printf("I do not know was ist das\n");
    return NULL;
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

        bsearch_info.mid  = (bsearch_info.left + bsearch_info.right) / 2;
        int res = (OPERATION_ARRAY[bsearch_info.mid].hash_command - djb_2_hash(current_command, bsearch_info.len_command));

        if (res == 0) {
            printf("BSEARCH_OPERATION = %s\n", OPERATION_ARRAY[bsearch_info.mid].op_name);
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


ssize_t find_variable(const char* current_var) {

    bsearch_t bsearch_info = {};
    bsearch_info.right = MAXLEN_VARIABLE_ARRAY - 1;
    bsearch_info.len_command = strlen(current_var);

    while(bsearch_info.left <= bsearch_info.right) { //equal to bsearch()

        bsearch_info.mid  = (bsearch_info.left + bsearch_info.right) / 2;
        int res = (OPERATION_ARRAY[bsearch_info.mid].hash_command - djb_2_hash(current_var, bsearch_info.len_command));

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