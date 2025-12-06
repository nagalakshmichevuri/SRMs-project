#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <ctype.h>
#include <stdbool.h>

#define STUDENT_FILE "students.txt"
#define CREDENTIAL_FILE "credentials.txt"

struct Student {
    int roll;
    char name[100];
    float marks;
};

char currentRole[20];
char currentUser[50];

/* ---------------- HIDDEN PASSWORD ---------------- */
void getHiddenPassword(char *pass) {
    char ch;
    int idx = 0;

    while (1) {
        ch = getch();

        if (ch == 13) {   // ENTER
            pass[idx] = '\0';
            printf("\n");
            break;
        }
        else if ((ch == 8 || ch == 127) && idx > 0) {  // BACKSPACE
            idx--;
            pass[idx] = '\0';
            printf("\b \b");
        }
        else if (ch == 8 || ch == 127) {
            // ignore
        }
        else {
            pass[idx++] = ch;
            printf("*");
        }
    }
}

/* ---------------- LOWERCASE ---------------- */
void toLowerString(char *s) {
    for (int i = 0; s[i]; i++)
        s[i] = tolower(s[i]);
}

/* ---------------- LOGIN SYSTEM ---------------- */
int loginSystem() {
    char username[50], password[50];
    char fileUser[50], filePass[50], fileRole[20];

    int attempts = 3;

    while (attempts > 0) {
        printf("\n=========== LOGIN ===========\n");
        printf("Username: ");
        scanf("%s", username);

        printf("Password: ");
        getHiddenPassword(password);

        FILE *fin = fopen(CREDENTIAL_FILE, "r");
        if (!fin) {
            printf("Error: %s not found!\n", CREDENTIAL_FILE);
            return 0;
        }

        while (fscanf(fin, "%s %s %s", fileUser, filePass, fileRole) == 3) {
            if (strcmp(username, fileUser) == 0 && strcmp(password, filePass) == 0) {
                strcpy(currentUser, fileUser);
                strcpy(currentRole, fileRole);
                fclose(fin);
                printf("\n✔ Login successful! Welcome, %s.\n", currentUser);
                return 1;
            }
        }

        fclose(fin);
        attempts--;
        printf("\n❌ Incorrect Username or Password!\n");
        printf("Attempts left: %d\n", attempts);
    }

    printf("\n🚫 Too many failed attempts. Access denied!\n");
    return 0;
}

/* ---------------- REGISTER USER ---------------- */
void registerUser() {
    if (strcmp(currentRole, "admin") != 0) {
        printf("Access denied! Only admin can register users.\n");
        return;
    }

    char username[50], password[50], role[10];

    printf("\n===== Register New User =====\n");

    printf("New Username: ");
    scanf("%s", username);

    printf("New Password: ");
    getHiddenPassword(password);

    do {
        printf("Role (admin/user): ");
        scanf("%s", role);
    } while (strcmp(role, "admin") != 0 && strcmp(role, "user") != 0);

    FILE *fout = fopen(CREDENTIAL_FILE, "a");
    fprintf(fout, "%s %s %s\n", username, password, role);
    fclose(fout);

    printf("User registered successfully!\n");
}

/* ---------------- ADD STUDENT ---------------- */
void addStudent() {
    struct Student s;
    FILE *fout = fopen(STUDENT_FILE, "a");

    if (!fout) {
        printf("Error opening student file!\n");
        return;
    }

    printf("\n===== Add Student =====\n");
    printf("Enter Roll: ");
    scanf("%d", &s.roll);

    getchar();
    printf("Enter Full Name: ");
    fgets(s.name, sizeof(s.name), stdin);
    s.name[strcspn(s.name, "\n")] = '\0';

    printf("Enter Marks: ");
    scanf("%f", &s.marks);

    fprintf(fout, "%d|%s|%f\n", s.roll, s.name, s.marks);
    fclose(fout);

    printf("Student added successfully!\n");
}

/* ---------------- VIEW STUDENTS ---------------- */
void viewStudents() {
    FILE *fin = fopen(STUDENT_FILE, "r");
    struct Student s;

    if (!fin) {
        printf("No student records found.\n");
        return;
    }

    printf("\n=========== STUDENT LIST ===========\n");

    while (fscanf(fin, "%d|%[^|]|%f\n", &s.roll, s.name, &s.marks) == 3) {
        printf("Roll: %d | Name: %s | Marks: %.2f\n", s.roll, s.name, s.marks);
    }

    fclose(fin);
}

/* ---------------- SEARCH STUDENT ---------------- */
void searchStudent() {
    FILE *fin = fopen(STUDENT_FILE, "r");
    struct Student s;

    if (!fin) {
        printf("No student file found!\n");
        return;
    }

    char inputName[100];
    bool found = false;

    getchar();
    printf("\nEnter name to search: ");
    fgets(inputName, sizeof(inputName), stdin);
    inputName[strcspn(inputName, "\n")] = '\0';

    toLowerString(inputName);

    printf("\n========== SEARCH RESULTS ==========\n");

    while (fscanf(fin, "%d|%[^|]|%f\n", &s.roll, s.name, &s.marks) == 3) {

        char tempName[100];
        strcpy(tempName, s.name);
        toLowerString(tempName);

        if (strstr(tempName, inputName) != NULL) {
            printf("Roll: %d | Name: %s | Marks: %.2f\n", s.roll, s.name, s.marks);
            found = true;
        }
    }

    if (!found)
        printf("No matching student found!\n");

    fclose(fin);
}

/* ---------------- DELETE STUDENT ---------------- */
void deleteStudent() {
    FILE *fin = fopen(STUDENT_FILE, "r");
    FILE *temp = fopen("temp.txt", "w");

    if (!fin) {
        printf("Student file not found!\n");
        return;
    }

    int roll;
    bool found = false;
    struct Student s;

    printf("Enter roll to delete: ");
    scanf("%d", &roll);

    while (fscanf(fin, "%d|%[^|]|%f\n", &s.roll, s.name, &s.marks) == 3) {
        if (s.roll == roll) {
            found = true;
            continue;
        }
        fprintf(temp, "%d|%s|%f\n", s.roll, s.name, s.marks);
    }

    fclose(fin);
    fclose(temp);

    remove(STUDENT_FILE);
    rename("temp.txt", STUDENT_FILE);

    if (found)
        printf("Record deleted successfully!\n");
    else
        printf("Record not found!\n");
}

/* ---------------- UPDATE STUDENT ---------------- */
void updateStudent() {
    FILE *fin = fopen(STUDENT_FILE, "r");
    FILE *temp = fopen("temp.txt", "w");

    if (!fin) {
        printf("Student file not found!\n");
        return;
    }

    int roll;
    bool found = false;
    struct Student s;

    printf("Enter roll to update: ");
    scanf("%d", &roll);

    getchar();

    while (fscanf(fin, "%d|%[^|]|%f\n", &s.roll, s.name, &s.marks) == 3) {

        if (s.roll == roll) {
            printf("\nEnter new full name: ");
            fgets(s.name, sizeof(s.name), stdin);
            s.name[strcspn(s.name, "\n")] = '\0';

            printf("Enter new marks: ");
            scanf("%f", &s.marks);

            getchar();
            found = true;
        }

        fprintf(temp, "%d|%s|%f\n", s.roll, s.name, s.marks);
    }

    fclose(fin);
    fclose(temp);

    remove(STUDENT_FILE);
    rename("temp.txt", STUDENT_FILE);

    if (found)
        printf("Record updated successfully!\n");
    else
        printf("Record not found!\n");
}

/* ---------------- PROFILE ---------------- */
void viewProfile() {
    printf("\n===== USER PROFILE =====\n");
    printf("Username: %s\n", currentUser);
    printf("Role: %s\n", currentRole);
}

/* ---------------- MAIN MENU ---------------- */
void mainMenu() {
    int choice;

    while (1) {
        printf("\n========= MAIN MENU (%s) =========\n", currentRole);
        printf("1. Add Student (Admin only)\n");
        printf("2. View Students\n");
        printf("3. Search Student (By Name)\n");
        printf("4. Update Student (Admin only)\n");
        printf("5. Delete Student (Admin only)\n");
        printf("6. Register User (Admin only)\n");
        printf("7. View Profile\n");
        printf("8. Logout\n");
        printf("Enter choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                if (strcmp(currentRole, "admin") == 0) addStudent();
                else printf("Access Denied!\n");
                break;

            case 2:
                viewStudents();
                break;

            case 3:
                searchStudent();
                break;

            case 4:
                if (strcmp(currentRole, "admin") == 0) updateStudent();
                else printf("Access Denied!\n");
                break;

            case 5:
                if (strcmp(currentRole, "admin") == 0) deleteStudent();
                else printf("Access Denied!\n");
                break;

            case 6:
                registerUser();
                break;

            case 7:
                viewProfile();
                break;

            case 8:
                return;

            default:
                printf("Invalid choice!\n");
        }
    }
}

/* ---------------- MAIN ---------------- */
int main() {

    if (loginSystem())
        mainMenu();
    else
        printf("\nAccess Denied. Exiting...\n");

    return 0;
}