#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#define NAME_LEN 20
#define CONTACT_LEN 20
#define MAX_SUPPLIERS 1000
#define FAILURE 1
#define SUCCESS 0

//temp supp array
typedef struct {
    int supplierID;
    int uniqueCount;
} SupplierEntry;

typedef struct {
    int supplierID;
    int turnover; // total cost = quantity supplied * price
} Supplier;

typedef struct ExpiryDate {
    int day;
    int month;
    int year;
}Date;

typedef struct Batch 
{
    int batchNumber;
    int quantity;
    struct ExpiryDate Date; 
    int totalSales;
    struct Batch *left, *right;
}Batch;

typedef struct Supplierinfo {
    int supplierID;
    char name[CONTACT_LEN];
    char contact[CONTACT_LEN];
    int quantitySupplied;
    struct Supplierinfo *left, *right;
}Supp;

typedef struct Medication {
    int medicationID;
    char name[NAME_LEN];
    float pricePerUnit;
    int totalQuantity;
    int reorderLevel;
    int height; 
    struct Batch *batchRoot;     
    struct Supplierinfo *supplierRoot; 
    int batchCount;  
    int supplierCount;
    struct Medication *left, *right;
}Medi;

//void inorderBatchTraversal(Batch *batchRoot);//search medication
void searchByMedName(Medi *root, char *name);//search mediation
void searchSupplierInBST(Supp* root, int suppID, Medi* medicine);//search mediation
void searchBySupplierID(Medi* root, int suppID);//search mediation
void searchMedication(Medi *root);
Batch* insertBatch(Batch* root, int batchNumber, Date expirationDate, int quantity);//add new medication
Supp* insertSupplier(Supp* root, int supplierID, char* name, char* contact, int quantitySupplied);//add new medication
int addNewMedication(Medi** root);
Medi* searchM(Medi* root,int id);//for medicine search
Supp* search(Supp* root,int id);//for supplier search
void updateSupplierInfo(Supp* supplier);//update medication
int updateMedication(Medi* root);
Medi* findMin(Medi* root);//dlt medication
Medi* delete(Medi* root, int id, int* ret_val);//dlt medicaiton
int deleteMedication(Medi** root);
void stockAlert(Medi* current);
void addSupplierToMedication(Medi* root);//supplier management
void updateSupplierInMedication(Medi* root);//supplier management
void searchSupplierInMedication(Medi* root);//supplier management
void supplierManagement(Medi *root);
int sales_tracking(Medi *root);
void checkExpirationDates(Medi *medRoot);
int compareDates(Date d1, Date d2);//check expiration date and sort expiration date
void sort_medication_by_expirationDate(Medi* medRoot);
void top_10_LargestTurnover(Medi* medRoot);
void top_10_rounderSupplier(Medi* medRoot);
void saveMedicationData(Medi* root);
Medi* loadMedicationData();
void saveBatchData(FILE* fp, Batch* root);
Batch* loadBatchData(FILE* fp);
void saveSupplierData(FILE* fp, Supp* root);
Supp* loadSupplierData(FILE* fp);

// Function to save batch data recursively
void saveBatchData(FILE* fp, Batch* root) {
    if (root == NULL) {
        fprintf(fp, "0\n"); // Mark end of subtree
        return;
    }
    //root->totalSales=0;
    fprintf(fp, "1\n"); // Mark node exists
    fprintf(fp, "%d %d %d %d %d %d\n", 
            root->batchNumber, root->quantity, root->totalSales,
            root->Date.day, root->Date.month, root->Date.year);
    
    saveBatchData(fp, root->left);
    saveBatchData(fp, root->right);
}

// Function to load batch data recursively
Batch* loadBatchData(FILE* fp) {
    int nodeExists;
    fscanf(fp, "%d", &nodeExists);
    if (!nodeExists) return NULL;
    
    Batch* newBatch = (Batch*)malloc(sizeof(Batch));
    fscanf(fp, "%d %d %d %d %d %d",
           &newBatch->batchNumber, &newBatch->quantity, &newBatch->totalSales,
           &newBatch->Date.day, &newBatch->Date.month, &newBatch->Date.year);
    
    newBatch->left = loadBatchData(fp);
    newBatch->right = loadBatchData(fp);
    return newBatch;
}

// Function to save supplier data recursively
void saveSupplierData(FILE* fp, Supp* root) {
    if (root == NULL) {
        fprintf(fp, "0\n"); // Mark end of subtree
        return;
    }
    
    fprintf(fp, "1\n"); // Mark node exists
    fprintf(fp, "%d %s %s %d\n", 
            root->supplierID, root->name, root->contact, root->quantitySupplied);
    
    saveSupplierData(fp, root->left);
    saveSupplierData(fp, root->right);
}

// Function to load supplier data recursively
Supp* loadSupplierData(FILE* fp) {
    int nodeExists;
    fscanf(fp, "%d", &nodeExists);
    if (!nodeExists) return NULL;
    
    Supp* newSupp = (Supp*)malloc(sizeof(Supp));
    fscanf(fp, "%d %s %s %d",
           &newSupp->supplierID, newSupp->name, newSupp->contact, &newSupp->quantitySupplied);
    
    newSupp->left = loadSupplierData(fp);
    newSupp->right = loadSupplierData(fp);
    return newSupp;
}

// Function to save medication data recursively
void saveMedicationDataRecursive(FILE* fp, Medi* root) {
    if (root == NULL) {
        fprintf(fp, "0\n"); // Mark end of subtree
        return;
    }
    
    fprintf(fp, "1\n"); // Mark node exists
    fprintf(fp, "%d %s %f %d %d %d\n",
            root->medicationID, root->name, root->pricePerUnit,
            root->reorderLevel, root->totalQuantity, root->height);
    
    // Save batch tree
    saveBatchData(fp, root->batchRoot);
    
    // Save supplier tree
    saveSupplierData(fp, root->supplierRoot);
    
    // Save left and right subtrees
    saveMedicationDataRecursive(fp, root->left);
    saveMedicationDataRecursive(fp, root->right);
}

// Main function to save medication data
void saveMedicationData(Medi* root) {
    FILE* fp = fopen("medication_data_avl.txt", "w");
    if (fp == NULL) {
        printf("Error opening file for writing medication data.\n");
        return;
    }
    
    saveMedicationDataRecursive(fp, root);
    fclose(fp);
    printf("Medication data saved successfully.\n");
}

Medi* loadMedicationDataRecursive(FILE* fp) {
    int nodeExists;
    fscanf(fp, "%d", &nodeExists);
    if (!nodeExists) return NULL;
    
    Medi* newMed = (Medi*)malloc(sizeof(Medi));
    fscanf(fp, "%d %s %f %d %d %d",
           &newMed->medicationID, newMed->name, &newMed->pricePerUnit,
           &newMed->reorderLevel, &newMed->totalQuantity, &newMed->height);
    
    // Load batch tree
    newMed->batchRoot = loadBatchData(fp);
    
    // Load supplier tree
    newMed->supplierRoot = loadSupplierData(fp);
    
    // Load left and right subtrees
    newMed->left = loadMedicationDataRecursive(fp);
    newMed->right = loadMedicationDataRecursive(fp);
    
    return newMed;
}

// Main function to load medication data
Medi* loadMedicationData() {
    FILE* fp = fopen("medication_data_avl.txt", "r");
    if (fp == NULL) {
        printf("No existing medication data found.\n");
        return NULL;
    }
    
    Medi* root = loadMedicationDataRecursive(fp);
    fclose(fp);
    printf("Medication data loaded successfully.\n");
    return root;
}

int uniqueMedCount[MAX_SUPPLIERS] = {0};
SupplierEntry supplierArray[MAX_SUPPLIERS];
int supplierTotal = 0;


int turnoverTable[MAX_SUPPLIERS] = {0};
Supplier supplierA[MAX_SUPPLIERS];
int supplierT = 0;
//
int compareDates(Date d1, Date d2) {
    if (d1.year != d2.year) return d1.year - d2.year;
    if (d1.month != d2.month) return d1.month - d2.month;
    return d1.day - d2.day;
}

Batch* insertBatch(Batch* root, int batchNumber, Date expirationDate, int quantity) {
    if (root == NULL) {
        Batch* newBatch = (Batch*)malloc(sizeof(Batch));
        newBatch->batchNumber = batchNumber;
        newBatch->Date = expirationDate;
        newBatch->quantity = quantity;
        newBatch->left = newBatch->right = NULL;
        return newBatch;
    }

    int cmp = compareDates(expirationDate, root->Date);
    if (cmp < 0) {
        root->left = insertBatch(root->left, batchNumber, expirationDate, quantity);
    } else if (cmp > 0) {
        root->right = insertBatch(root->right, batchNumber, expirationDate, quantity);
    } else {
        printf("Batch number %d already exists. Duplicate entries are not allowed.\n", batchNumber);
    }

    return root;
}
//remain as it is
Supp* insertSupplier(Supp* root, int supplierID, char* name, char* contact, int quantitySupplied) 
{
    if (root == NULL) 
    {
        Supp* newSupplier = (Supp*)malloc(sizeof(Supp));
        newSupplier->supplierID = supplierID;
        strcpy(newSupplier->name, name);
        strcpy(newSupplier->contact, contact);
        newSupplier->quantitySupplied = quantitySupplied;
        newSupplier->left = newSupplier->right = NULL;
        return newSupplier;
    }
    if (supplierID < root->supplierID)
    {
        root->left = insertSupplier(root->left, supplierID, name, contact, quantitySupplied);
    }
    else if(supplierID > root->supplierID)
    {
        root->right = insertSupplier(root->right, supplierID, name, contact, quantitySupplied);
    }
    else
    {
        printf("Supplier ID %d already exists.Duplicate entries are not allowed.\n",supplierID);
    }
    return root;
}
int max(int a, int b) 
{
    return (a > b) ? a : b;
}
int height(Medi* node) 
{
    if (node == NULL)
        return 0;
    return node->height;
}

int getBalanceFactor(Medi* node) 
{
    if (node == NULL)
        return 0;
    return height(node->left) - height(node->right);
}

Medi* rightRotate(Medi* y) 
{
    Medi* x = y->left;
    Medi* T2 = x->right;
    x->right = y;
    y->left = T2;

    y->height = 1 + max(height(y->left), height(y->right));
    x->height = 1 + max(height(x->left), height(x->right));
    return x; 
}

Medi* leftRotate(Medi* x) 
{
    Medi* y = x->right;
    Medi* T2 = y->left;
    y->left = x;
    x->right = T2;

    x->height = 1 + max(height(x->left), height(x->right));
    y->height = 1 + max(height(y->left), height(y->right));
    return y;
}

Medi* insertMedication(Medi* root, int id, char* name, float price, int reorderLevel, 
    Batch batchList[], int batchCount, Supp supplierList[], int supplierCount,int  totalQuantity) 
{  
    if(root==NULL)
    {
        Medi* newMed = (Medi*)malloc(sizeof(Medi));
        newMed->medicationID = id;
        strcpy(newMed->name, name);
        newMed->pricePerUnit = price;
        newMed->reorderLevel = reorderLevel;
        newMed->batchRoot = NULL;
        newMed->supplierRoot = NULL;
        newMed->left = newMed->right = NULL;
        newMed->totalQuantity= totalQuantity;

        for (int i = 0; i < batchCount; i++) 
        {
            newMed->batchRoot = insertBatch(newMed->batchRoot, batchList[i].batchNumber, batchList[i].Date, batchList[i].quantity);
        }

        for (int i = 0; i < supplierCount; i++) 
        {
            newMed->supplierRoot = insertSupplier(newMed->supplierRoot, supplierList[i].supplierID, supplierList[i].name, supplierList[i].contact, batchList[i].quantity);
        }
        newMed->height = 1;
        return newMed;
    }
    if (id < root->medicationID)
    {
        root->left = insertMedication(root->left, id, name, price, reorderLevel, batchList, batchCount, supplierList, supplierCount, totalQuantity);
    }
    else if (id > root->medicationID)
    {
        root->right = insertMedication(root->right, id, name, price, reorderLevel, batchList, batchCount, supplierList, supplierCount, totalQuantity);
    }
    else
    {
        printf("Medication ID %d already exists.Duplicate entries are not allowed.\n",id);
    }

    root->height = 1 + max(height(root->left), height(root->right));

    int balance = getBalanceFactor(root);

    if (balance > 1 && id < root->left->medicationID)
    {
        return rightRotate(root);
    } 
    if (balance < -1 && id > root->right->medicationID)
    {
        return leftRotate(root);
    }
    // Left-Right
    if (balance > 1 && id > root->left->medicationID) 
    {
        root->left = leftRotate(root->left);
        return rightRotate(root);
    }
    // Right-Left
    if (balance < -1 && id < root->right->medicationID) 
    {
        root->right = rightRotate(root->right);
        return leftRotate(root);
    }
    return root;
}
//

int addNewMedication(Medi** root) 
{
    int id, reorderLevel, batchCount, supplierCount;
    int totalQuantity = 0;
    char name[NAME_LEN];
    float price;
    int result = SUCCESS;

    printf("\nEnter Medication ID: ");
    scanf("%d", &id);
    printf("Enter Medication Name: ");
    scanf(" %s", name);
    printf("Enter Price per Unit: ");
    scanf("%f", &price);
    printf("Enter Reorder Level: ");
    scanf("%d", &reorderLevel);
    printf("Enter number of batches: ");
    scanf("%d", &batchCount);

    Batch* batchList = (Batch*)malloc(batchCount * sizeof(Batch));  
    if (batchList == NULL) result = FAILURE;

    for (int i = 0; i < batchCount && result == SUCCESS; i++) 
    {  
        printf("\nBatch %d:\n", i + 1);
        printf("Enter Batch Number: ");
        scanf("%d", &batchList[i].batchNumber);
        printf("Enter Expiration Date (DD MM YYYY): ");
        scanf("%d %d %d", &batchList[i].Date.day, &batchList[i].Date.month, &batchList[i].Date.year);
        printf("Enter Quantity: ");
        scanf("%d", &batchList[i].quantity);
        totalQuantity += batchList[i].quantity;
        batchList[i].totalSales=0;
    }

    printf("Enter number of suppliers: ");
    scanf("%d", &supplierCount);
    Supp* supplierList = (Supp*)malloc(supplierCount * sizeof(Supp));  
    if (supplierList == NULL) result = FAILURE;

    for (int i = 0; i < supplierCount && result == SUCCESS; i++) 
    {  
        printf("\nSupplier %d:\n", i + 1);
        printf("Enter Supplier ID: ");
        scanf("%d", &supplierList[i].supplierID);
        printf("Enter Supplier Name: ");
        scanf(" %s", supplierList[i].name);
        printf("Enter Contact Info: ");
        scanf(" %s", supplierList[i].contact);
        printf("Enter Quantity Supplied: ");
        scanf("%d", &supplierList[i].quantitySupplied);
    }
    if (result == SUCCESS) 
    {
        *root = insertMedication(*root, id, name, price, reorderLevel, batchList, batchCount, supplierList, supplierCount, totalQuantity);
        printf("\nMedication added successfully!\n");
    } else
    {
        printf("\nError: Memory allocation failed. Could not add medication.\n");
    }
    if (batchList) free(batchList);
    if (supplierList) free(supplierList);
    return result;
}

//
Medi* searchM(Medi* root,int id)
{
    Medi* ret_Node=NULL;
    if(root!=NULL)
    {
        if(root->medicationID==id)
        {
            ret_Node=root;
        }
        else if(root->medicationID>id)
        {
            ret_Node=searchM(root->left,id);
        }
        else
        {
            ret_Node=searchM(root->right,id);
        }
    }
    return ret_Node;
}

void updateSupplierInfo(Supp* supplier)
{
    if (supplier == NULL) return;

    printf("Updating Supplier (ID: %d)\n", supplier->supplierID);

    printf("Enter updated Supplier ID: ");
    scanf("%d", &supplier->supplierID);

    printf("Enter updated Supplier Name: ");
    scanf("%s", supplier->name);

    printf("Enter updated Contact Info: ");
    scanf("%s", supplier->contact);

    updateSupplierInfo(supplier->left);
    updateSupplierInfo(supplier->right);
}

//
int updateMedication(Medi* root) 
{
    int ret_val = SUCCESS;
    int id;
    printf("Enter the medication ID you want to update: ");
    scanf("%d", &id);

    Medi* current = searchM(root, id); 

    if (current != NULL) 
    {
        int option;
        float newPrice;
        int newReorderLevel;
        int newTotalQuantity;

        printf("Enter 1 to modify price\n");
        printf("Enter 2 to modify reorder level\n");
        printf("Enter 3 to modify total quantity\n");
        printf("Enter 4 to modify supplier info\n");
        scanf("%d", &option);

        if (option == 1) 
        {
            printf("Enter new price: ");
            scanf("%f", &newPrice);
            current->pricePerUnit = newPrice;
        }
        else if (option == 2) 
        {
            printf("Enter new reorder level: ");
            scanf("%d", &newReorderLevel);
            current->reorderLevel = newReorderLevel;
        }
        else if (option == 3) 
        {
            printf("Enter new total quantity: ");
            scanf("%d", &newTotalQuantity);
            current->totalQuantity = newTotalQuantity;
        }
        else if (option == 4) 
        {
            updateSupplierInfo(current->supplierRoot);
        }
        else
        {
            printf("Invalid option.\n");
            ret_val = FAILURE;
        }

        printf("Medication details updated successfully.\n");
    } 
    else 
    {
        printf("Medication with ID %d not found.\n", id);
        ret_val = FAILURE;
    }

    return ret_val;
}

Medi* findMin(Medi* root) 
{
    Medi* ret_val;
    if(root==NULL)
    {
        ret_val=NULL;
    }
    else
    {
        while (root->left != NULL)
       {
            root = root->left;
            ret_val=root;
       }
    }
    return ret_val;
}

void freeBatchTree(Batch* root) 
{
    if (root == NULL)
    {
        return;
    }
    freeBatchTree(root->left);  
    freeBatchTree(root->right);
    free(root);
}

void freeSupplierTree(Supp* root) 
{
    if (root == NULL)
    {
        return;
    }
    freeSupplierTree(root->left);
    freeSupplierTree(root->right); 
    free(root);
}
//
Medi* delete(Medi* root, int id, int* ret_val) 
{
    if (root == NULL) 
    {
        printf("Medication with ID %d not found.\n", id);
        *ret_val = FAILURE;
        return NULL;
    }

    // Step 1: Standard BST delete
    if (id < root->medicationID) 
    {
        root->left = delete(root->left, id, ret_val);
    } 
    else if (id > root->medicationID) 
    {
        root->right = delete(root->right, id, ret_val);
    } 
    else 
    {
        // Step 2: Free all batches
        freeBatchTree(root->batchRoot);
        root->batchRoot = NULL;

        // Step 3: Free all suppliers
        freeSupplierTree(root->supplierRoot);
        root->supplierRoot = NULL;

        // Step 4: Handle 0, 1, or 2 children cases
        if (root->left == NULL || root->right == NULL) 
        {
            Medi* temp = (root->left) ? root->left : root->right;

            if (temp == NULL) 
            {
                // No children case
                temp = root;
                root = NULL;
            } 
            else 
            {
                // One child case
                *root = *temp;
            }

            free(temp);
        } 
        else 
        {
            // Two children case: Find in-order successor
            Medi* temp = findMin(root->right);

            // Copy successor data
            root->medicationID = temp->medicationID;
            strcpy(root->name, temp->name);
            root->pricePerUnit = temp->pricePerUnit;
            root->totalQuantity = temp->totalQuantity;
            root->reorderLevel = temp->reorderLevel;
            root->batchRoot = temp->batchRoot;
            root->supplierRoot = temp->supplierRoot;
            root->batchCount = temp->batchCount;
            root->supplierCount = temp->supplierCount;

            // Delete in-order successor
            root->right = delete(root->right, temp->medicationID, ret_val);
        }

        printf("Medication with ID %d deleted successfully.\n", id);
        *ret_val = SUCCESS;
    }

    if (root == NULL)
        return NULL;

    // Step 5: Update height
    root->height = 1 + ((height(root->left) > height(root->right)) ? height(root->left) : height(root->right));

    // Step 6: Balance the tree
    int balance = getBalanceFactor(root);

    // Left Heavy (LL or LR case)
    if (balance > 1) 
    {
        if (getBalanceFactor(root->left) >= 0)
            return rightRotate(root);
        else 
        {
            root->left = leftRotate(root->left);
            return rightRotate(root);
        }
    }
    // Right Heavy (RR or RL case)
    if (balance < -1) 
    {
        if (getBalanceFactor(root->right) <= 0)
            return leftRotate(root);
        else 
        {
            root->right = rightRotate(root->right);
            return leftRotate(root);
        }
    }

    return root;
}
//
int deleteMedication(Medi** root) 
{
    int id;
    printf("Enter the Medication ID to delete: ");
    scanf("%d", &id);
    int ret_val = FAILURE;  
    *root = delete(*root, id, &ret_val); 
    if (ret_val == SUCCESS) 
    {
        printf("Medication with ID %d deleted successfully.\n", id);
    } 
    else 
    {
        printf("Medication with ID %d not found.\n", id);
    }
    return ret_val;
}

//
void stockAlert(Medi* current)
{
    printf("checking stock levels....\n");
    if(current->totalQuantity<=current->reorderLevel)
    {
        printf("Alert : Medication '% s'(ID:%d)needs restocking! \n",current->name,current->medicationID);
        printf("current stock : %d,\n Reorder Level : %d\n",current->totalQuantity,current->reorderLevel);
    }
    else
    {
        printf("Quantity in stock is sufficient");
    }
}
//
void addSupplierToMedication(Medi* root) 
{
    int id,suppID;
    printf("Enter Medication ID to add a supplier: ");
    scanf("%d", &id);
    Medi* current=searchM(root,id);
    if(current!=NULL)
    {
        Supp* newSupplier = (Supp*)malloc(sizeof(Supp));
        printf("Enter Supplier ID: ");
        scanf("%d",&suppID);
        newSupplier->supplierID=suppID;
        printf("Enter Supplier Name: ");
        scanf("%s", newSupplier->name);
        printf("Enter Supplier Contact Info: ");
        scanf("%s", newSupplier->contact);
        printf("Enter Quantity Supplied: ");
        scanf("%d", &newSupplier->quantitySupplied);
        current->supplierRoot=insertSupplier(current->supplierRoot,newSupplier->supplierID,newSupplier->name, newSupplier->contact, newSupplier->quantitySupplied);
        printf("Supplier added successfully to Medication ID: %d.\n", id); 
    }
    else
    {
        printf("Medication not found.\n");
    }
}
//
Supp* search(Supp* root,int id)
{
    Supp* ret_Node=NULL;
    if(root!=NULL)
    {
        if(root->supplierID==id)
        {
            ret_Node=root;
        }
        else if(root->supplierID > id)
        {
            ret_Node=search(root->left,id);
        }
        else
        {
            ret_Node=search(root->right,id);
        }
    }
    return ret_Node;
}

//
void updateSupplierInMedication(Medi* root) 
{
    int id;
    printf("Enter Medication ID to update a supplier: ");
    scanf("%d", &id);
    Medi* med =searchM(root,id);
    if(med!=NULL)
    {   
        int suppID;
        printf("Enter Supplier ID to update: ");
        scanf("%d", &suppID);
        Supp* supp=search(med->supplierRoot,suppID);
        if(supp!=NULL)
        {
            printf("Enter updated Supplier Name: ");
            scanf("%s", supp->name);
            printf("Enter updated Contact Info: ");
            scanf("%s", supp->contact);
            printf("Enter updated Quantity Supplied: ");
            scanf("%d", &supp->quantitySupplied);
            printf("Supplier updated successfully for Medication ID: %d.\n", id);
        }
        else
        {
           printf("supplier not found.\n");
        }
    }
    else{
        printf("medication not found.\n");
    }
}
//
void searchSupplierInMedication(Medi* root) 
{
    int id, suppID;
    printf("Enter Medication ID to search for a supplier: ");
    scanf("%d", &id);
    Medi* current=searchM(root,id);
    if(current!=NULL)
    {
        printf("Enter Supplier ID to search: ");
        scanf("%d", &suppID);
        Supp* suppTemp=search(current->supplierRoot,suppID);
        if(suppTemp!=NULL)
        {
            printf("Supplier ID: %d\n",suppTemp->supplierID);
            printf("Supplier Name: %s\n",suppTemp->name);
            printf("Contact Info: %s\n",suppTemp->contact);
            printf("Quantity Supplied: %d\n",suppTemp->quantitySupplied);
        }
        else
        {
            printf("Supplier not found.\n");
        }
    }
    else
    {
        printf("Medication not found.\n");
    }

}
//
void supplierManagement(Medi *root) 
{
    int choice;
    do {
        printf("Supplier Management Menu:\n");
        printf("1. Add Supplier\n");
        printf("2. Update Supplier\n");
        printf("3. Search Supplier\n");
        printf("4. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                addSupplierToMedication(root);
                break;
            case 2:
                updateSupplierInMedication(root);
                break;
            case 3:
                searchSupplierInMedication(root);
                break;
            case 4:
                printf("Exiting...\n");
                break;
            default:
                printf("Invalid choice!\n");
        }
    } while (choice != 4);
}
Batch* findMinBatch(Batch* root) 
{
    Batch* ret_val;
    if(root==NULL)
    {
        ret_val=NULL;
    }
    else
    {
        while (root->left != NULL&&root->left->quantity!=0)
       {
            root = root->left;
            ret_val=root;
       }
    }
    return ret_val;
}
//
int sales_tracking(Medi *root) 
{   
    int ret_val = SUCCESS;
    int quantity, medID;

    printf("Enter the ID of the medicine you want to buy: \n");
    scanf("%d", &medID);
    printf("Enter the quantity: \n");
    scanf("%d", &quantity);

    int remainingQty = quantity;
    int batchQty = 0;

    Medi* current = searchM(root, medID); 

    if (current != NULL) 
    {
        printf("Medication found: %d\n", current->medicationID);
        while (remainingQty > 0 && current->batchRoot != NULL)
        {
            Batch* batchTemp = findMinBatch(current->batchRoot);

            if (batchTemp == NULL) 
            {
                printf("Error: Not enough stock to fulfill %d units. Only partial order fulfilled.\n", quantity - remainingQty);
                remainingQty = 0;
            } 
            else 
            {
                batchQty = batchTemp->quantity;

                if (remainingQty <= batchQty) 
               {
                    batchTemp->quantity -= remainingQty;
                    batchTemp->totalSales += remainingQty;
                    current->totalQuantity -= remainingQty;
                    printf("Sale completed for %d units from batch %d.\n", remainingQty, batchTemp->batchNumber);
                    stockAlert(current);
                    remainingQty = 0; 
                } 
                else 
                {
                    printf("Depleting batch %d: %d units sold.\n", batchTemp->batchNumber, batchQty);
                    remainingQty -= batchQty;
                    batchTemp->totalSales += batchQty;
                    batchTemp->quantity = 0;
                    current->totalQuantity -= batchQty;
                    stockAlert(current);
                    if (batchTemp->right != NULL && current->batchRoot->quantity == 0) 
                    {
                        printf("All batches in the left subtree are empty. Moving to right subtree batch...\n");
                        batchTemp = findMinBatch(batchTemp->right);
                    }
                    else 
                    {
                        printf("No more batches left in stock.\n");
                        remainingQty = 0;
                    }
                }
            } 
        }
    }
    else
    {
        printf("Error: Medication with ID %d not found.\n",medID);
        ret_val=FAILURE;
    }
    return ret_val;
}
//
// void inorderBatchTraversal(Batch *batchRoot)
// {
//     if (batchRoot != NULL)
//     {
//         inorderBatchTraversal(batchRoot->left);  
//         printf("Batch No: %s, Quantity in Stock: %d\n", batchRoot->batchNumber, batchRoot->quantity);
//         printf(" Expiration Date:%d %d %d\n",batchRoot->Date.day,batchRoot->Date.month,batchRoot->Date.year);;
//         inorderBatchTraversal(batchRoot->right); 
//     }
// }
//
void searchByMedName(Medi *root, char *name) 
{
    if (root != NULL) 
    {
        searchByMedName(root->left, name);  
        if (strcmp(root->name, name) == 0) 
        {
            printf("Medication found:\n");
            printf("ID: %d\nName: %s\nPrice per Unit: %.2f\nReorder Level: %d\n",
                   root->medicationID, root->name, root->pricePerUnit, root->reorderLevel);
            // Display all batches
            //inorderBatchTraversal(root->batchRoot);
        }
        searchByMedName(root->right, name);
    }
}
//
void searchSupplierInBST(Supp* root, int suppID, Medi* medicine) 
{
    if(root!=NULL)
    {
        searchSupplierInBST(root->left, suppID, medicine);
        if (root->supplierID == suppID) 
        {
            printf("Supplier found: %d\n", root->supplierID);
            printf("Medication Name: %s, Price per Unit: %.2f, Reorder Level: %d\n",
               medicine->name, medicine->pricePerUnit, medicine->reorderLevel);
               //display all batches
            //inorderBatchTraversal(medicine->batchRoot);
        }
        searchSupplierInBST(root->right, suppID, medicine);
    }
}
//
void searchBySupplierID(Medi* root, int suppID) 
{
    if(root!=NULL)
    {
        searchBySupplierID(root->left, suppID);
        // Search supplier BST of current medication
        searchSupplierInBST(root->supplierRoot, suppID, root);
        searchBySupplierID(root->right, suppID); 
    }
}

//search
void searchMedication(Medi *root) 
{   
    int statuscode=SUCCESS;
    int Option;
    printf("Enter 1 to search by Medication ID,\n enter 2 to search by Medication Name,\n enter 3 to search by Supplier ID ");
    printf("Enter your option: ");
    scanf("%d", &Option);

    switch (Option) {
        case 1: {
            int id;
            printf("Enter Medication ID: ");
            scanf("%d", &id);
            Medi *current = searchM(root,id); 
            if(current !=NULL)
            {
                printf("Medication found:\n");
                    printf("ID: %d\nName: %s,\nPrice per Unit: %f,\nReorder Level: %d,\n",
                           current->medicationID, current->name,
                           current->pricePerUnit,current->reorderLevel);
                    // printf("Batches of medicine and quantity in stock according to batch no.");
                    // inorderBatchTraversal(root->batchRoot); 
            }
            else{
                printf("medication not found");
            }
            break;
        }

        case 2: {
            char name[NAME_LEN];
            printf("Enter Medication Name: ");
            scanf("%s",name);
            searchByMedName(root, name);
            break;
        }

        case 3: {
            int suppID;
            printf("Enter Supplier ID: ");
            scanf("%d",&suppID);
            searchBySupplierID(root,suppID);
            }
        default:
            printf("Invalid search option!\n");
            break;
        }    
}
//checkExpirationDates
int daysInMonth(int month) 
{
    int daysPerMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    return daysPerMonth[month - 1];
}

int calculateDateDifference(Date date1, Date date2) 
{
    int days1 = 0, days2 = 0;
    days1 = date1.year * 365 + date1.day;
    for (int i = 1; i < date1.month; i++) {
        days1 += daysInMonth(i);
    }
    days2 = date2.year * 365 + date2.day;
    for (int i = 1; i < date2.month; i++) {
        days2 += daysInMonth(i);
    }
    return days2 - days1;
}

void checkBatchExpiration(Batch *root, Date today, const char *medicineName) 
{
    if(root!=NULL)
    {
        checkBatchExpiration(root->left, today, medicineName);
        int daysToExpiry = calculateDateDifference(today, root->Date);
        if (daysToExpiry > 0 && daysToExpiry <= 30) 
        {
            printf("ALERT: Medicine '%s', Batch No %d is expiring in %d days!\n",
               medicineName, root->batchNumber, daysToExpiry);
        } 
        else if (daysToExpiry <= 0) 
        {
        printf("EXPIRED: Medicine '%s', Batch No %d has already expired!\n",
               medicineName, root->batchNumber);
        }
        checkBatchExpiration(root->right, today, medicineName);
    }
}

void checkAllExpirations(Medi *medRoot, Date today) 
{
    if(medRoot!=NULL)
    {
        checkAllExpirations(medRoot->left, today);
        checkBatchExpiration(medRoot->batchRoot, today, medRoot->name);
        checkAllExpirations(medRoot->right, today);
    }
}
//
void checkExpirationDates(Medi *medRoot) 
{
    Date today;
    printf("Enter today's date (DD MM YYYY): ");
    scanf("%d %d %d", &today.day, &today.month, &today.year);

    printf("\nChecking for expiring and expired Medicines.\n");
    checkAllExpirations(medRoot, today);
}

void traverseSupplier(struct Supplierinfo* root) {
    if (root == NULL) return;

    // In-order traversal to visit each supplier
    traverseSupplier(root->left);

    if (root->supplierID >= 0 && root->supplierID < MAX_SUPPLIERS) {
        uniqueMedCount[root->supplierID]++;
    }

    traverseSupplier(root->right);
}

void traverseMedicineAVL(struct Medication* root) {
    if (root == NULL) return;

    traverseMedicineAVL(root->left);

    // For current medicine, traverse its supplier BST
    traverseSupplier(root->supplierRoot);  // assuming suppliers is BST root

    traverseMedicineAVL(root->right);
}

int partition1(SupplierEntry suppliers[], int lo, int hi) {
    int pivot = suppliers[lo].uniqueCount;
    int i = lo + 1;
    int j = hi;

    while (i <= j) {
        if (suppliers[i].uniqueCount >= pivot) {
            i++;
        } else if (suppliers[j].uniqueCount < pivot) {
            j--;
        } else {
            SupplierEntry temp = suppliers[i];
            suppliers[i] = suppliers[j];
            suppliers[j] = temp;
            i++;
            j--;
        }
    }
    SupplierEntry temp = suppliers[j];
    suppliers[j] = suppliers[lo];
    suppliers[lo] = temp;

    return j;
}

void quickSort1(SupplierEntry suppliers[], int lo, int hi) {
    if (lo < hi) {
        int p = partition1(suppliers, lo, hi);
        quickSort1(suppliers, lo, p - 1);
        quickSort1(suppliers, p + 1, hi);
    }
}

void top_10_rounderSupplier(Medi* medRoot) {
    // Step 1: Traverse AVL tree of medicines and supplier BST inside each
    traverseMedicineAVL(medRoot);

    // Step 2: Fill supplierArray[] from uniqueMedCount[]
    for (int i = 0; i < MAX_SUPPLIERS; i++) {
        if (uniqueMedCount[i] > 0) {
            supplierArray[supplierTotal].supplierID = i;
            supplierArray[supplierTotal].uniqueCount = uniqueMedCount[i];
            supplierTotal++;
        }
    }

    // Step 3: Sort supplierArray using quickSort2
    quickSort1(supplierArray, 0, supplierTotal - 1);

    // Step 4: Display results
    printf("\nTop 10 All-Rounder Suppliers:\n");
    for (int i = 0; i < supplierTotal && i < 10; i++) {
        printf("Supplier ID: %d, Unique Medications Supplied: %d\n",
               supplierArray[i].supplierID, supplierArray[i].uniqueCount);
    }

    if (supplierTotal > 0) {
        printf("\nBest All-Rounder Supplier:\n");
        int i=0;
        while(i < supplierTotal && supplierArray[0].uniqueCount==supplierArray[i].uniqueCount)
        {
            printf("Supplier ID: %d, Unique Medications Supplied: %d\n",
                supplierArray[0].supplierID, supplierArray[0].uniqueCount);
                i++;
        }
        
    }
}

// Traverse the supplier BST for each medicine
void traverseSupplierTurnoverBST(Supp* root, int medPrice) {
    if (root == NULL) return;

    traverseSupplierTurnoverBST(root->left, medPrice);

    if (root->supplierID >= 0 && root->supplierID < MAX_SUPPLIERS) {
        turnoverTable[root->supplierID] += (root->quantitySupplied * medPrice);
    }

    traverseSupplierTurnoverBST(root->right, medPrice);
}

// Traverse AVL of medicines
void traverseMedicineAVL_Turnover(Medi* root) {
    if (root == NULL) return;

    traverseMedicineAVL_Turnover(root->left);

    traverseSupplierTurnoverBST(root->supplierRoot, root->pricePerUnit);

    traverseMedicineAVL_Turnover(root->right);
}

// Reuse your custom quickSort2
int partition2(Supplier suppliers[], int lo, int hi) {
    int pivot = suppliers[lo].turnover;
    int i = lo + 1;
    int j = hi;

    while (i <= j) {
        if (suppliers[i].turnover >= pivot) {
            i++;
        } else if (suppliers[j].turnover < pivot) {
            j--;
        } else {
            Supplier temp = suppliers[i];
            suppliers[i] = suppliers[j];
            suppliers[j] = temp;
            i++;
            j--;
        }
    }
    Supplier temp = suppliers[j];
    suppliers[j] = suppliers[lo];
    suppliers[lo] = temp;

    return j;
}

void quickSort2(Supplier suppliers[], int lo, int hi) {
    if (lo < hi) {
        int p = partition2(suppliers, lo, hi);
        quickSort2(suppliers, lo, p - 1);
        quickSort2(suppliers, p + 1, hi);
    }
}

void top_10_LargestTurnover(Medi* medRoot) {
    traverseMedicineAVL_Turnover(medRoot);

    for (int i = 0; i < MAX_SUPPLIERS; i++) {
        if (turnoverTable[i] > 0) {
            supplierA[supplierT].supplierID = i;
            supplierA[supplierT].turnover = turnoverTable[i];
            supplierT++;
        }
    }

    quickSort2(supplierA, 0, supplierT - 1);

    printf("\nTop 10 Suppliers with Largest Turn-Over:\n");
    for (int i = 0; i < supplierT && i < 10; i++) {
        printf("Supplier ID: %d, Total Turn-Over: %d\n",
               supplierA[i].supplierID, supplierA[i].turnover);
    }

    if (supplierT> 0) {
        printf("\nBest Turn-Over Supplier(s):\n");
        int topTurnover = supplierA[0].turnover;
        int i = 0;
        while (i < supplierT && supplierA[i].turnover == topTurnover) {
                printf("Supplier ID: %d, Total Turn-Over: %d\n",
               supplierA[i].supplierID, supplierA[i].turnover);
                i++;
            }
    }

}
//sort_medication_by_expiry_date
void BatchesInDateRange(Batch* root, Date start, Date end) 
{
    if(root!=NULL)
    {
        if (compareDates(root->Date, start) >= 0)
        {
            BatchesInDateRange(root->left, start, end);
        }
        if (compareDates(root->Date, start) >= 0 && compareDates(root->Date, end) <= 0) 
        {
           printf("Batch Number: %d, Expiry Date: %02d-%02d-%04d, Quantity: %d\n",
           root->batchNumber, root->Date.day, root->Date.month, root->Date.year,
           root->quantity);
        }
        if (compareDates(root->Date, end) <= 0)
        {
            BatchesInDateRange(root->right, start, end);
        }
    }
}

void searchAllMedicationsByDateRange(Medi* root, Date start, Date end) 
{
    if(root!=NULL)
    {
        searchAllMedicationsByDateRange(root->left, start, end);
        printf("Medicine ID: %d, Name: %s\n", root->medicationID, root->name);
        BatchesInDateRange(root->batchRoot, start, end);
        searchAllMedicationsByDateRange(root->right, start, end);
    }
}
//
void sort_medication_by_expirationDate(Medi* medRoot) 
{
    Date startDate, endDate;
    printf("Enter Start Date: ");
    scanf("%d %d %d", &startDate.day, &startDate.month, &startDate.year);
    printf("Enter End Date: ");
    scanf("%d %d %d", &endDate.day, &endDate.month, &endDate.year);
    printf("\nMedications with batches expiring between %02d-%02d-%04d and %02d-%02d-%04d:\n",
           startDate.day, startDate.month, startDate.year,
           endDate.day, endDate.month, endDate.year);
    searchAllMedicationsByDateRange(medRoot, startDate, endDate);
}

int main() {
    //Medi* mediRoot = NULL; 
    Medi* mediRoot = loadMedicationData();
    int choice;
    int statusCode=SUCCESS;
    //traverse(mediRoot);
    do {
        printf("\n========================================\n");
        printf("Pharmacy Inventory Management System\n");
        printf("1. Add New Medication\n");
        printf("2. Update Medication Details\n");
        printf("3. Delete Medication\n");
        printf("4. Search Medication\n");
        printf("5. Check Expiration Dates\n");
        printf("6. Sort Medication by Expiration Date\n");
        printf("7. Sales Tracking\n");
        printf("8. Supplier Management\n");
        printf("9. Find All-rounder Suppliers\n");
        printf("10. Find Suppliers with Largest Turnover\n");
        printf("0. Exit\n");
        printf("========================================\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                printf("\nAdding New Medication...\n");
                statusCode= addNewMedication(&mediRoot);
                break;
            case 2:
                printf("\nUpdating Medication Details...\n");
                statusCode=updateMedication(mediRoot);
                break;
            case 3:
                printf("\nDeleting Medication...\n");
                statusCode= deleteMedication(&mediRoot);
                break;
            case 4:
                printf("\nSearching Medication...\n");
                searchMedication(mediRoot);
                break;
            case 5:
                printf("\nChecking Expiration Dates...\n");
                checkExpirationDates(mediRoot);
                break;
            case 6:
                printf("\nSorting Medications by Expiration Date...\n");
                sort_medication_by_expirationDate(mediRoot);
                break;
            case 7:
                printf("\nSales Tracking...\n");
                statusCode=sales_tracking(mediRoot);
                break;
            case 8:
                printf("\nManaging Supplier Information...\n");
                supplierManagement(mediRoot);
                break;
            case 9:
                printf("\nFinding All-Rounder Suppliers...\n");
                top_10_rounderSupplier(mediRoot);
                break;
            case 10:
                printf("\nFinding Suppliers with Largest Turnover...\n");
                top_10_LargestTurnover(mediRoot);
                break;
            case 0:
                printf("\nExiting system. Goodbye!\n");
                saveMedicationData(mediRoot);
                break;
            default:
                printf("\nInvalid choice. Please try again.\n");
        }
    } while (choice != 0);

    return 0;
}
