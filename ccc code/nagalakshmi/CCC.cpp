#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>   // for sleep()
#include <conio.h>    // for getch() -> hide password input

#define STUDENT_FILE "students.txt"

struct Student {
    int roll;
    char name[50];
    float marks;
};

// ===================== FUNCTION DECLARATIONS =====================
void adminMenu();
void staffMenu();
void guestMenu();

void addStudent();
void displayStudents();
void searchStudent();
void updateStudent();
void deleteStudent();
int login(char *role);

int rollExists(int roll);

// ======= NEW (ONLY FOR HIDING PASSWORD) =======
void readPassword(char *pass, int maxLen);

// ===================== MAIN FUNCTION =====================
int main() {
    char role[10];

    printf("===== Welcome to the Student Management System =====\n");
    printf("Login as (admin/staff/guest): ");
    scanf("%9s", role);

    if (login(role)) {
        if (strcmp(role, "admin") == 0) {
            adminMenu();
        } else if (strcmp(role, "staff") == 0) {
            staffMenu();
        } else if (strcmp(role, "guest") == 0) {
            guestMenu();
        } else {
            printf("Invalid role!\n");
        }
    } else {
        printf("\nToo many failed attempts! Program locked for 5 seconds...\n");
        sleep(5);
    }

    printf("\nsession ended!! See you again. Goodbye!!\n");
    return 0;
}

// ===================== NEW HELPER: HIDE PASSWORD =====================
// Reads password without showing characters (prints '*' instead)
void readPassword(char *pass, int maxLen) {
    int i = 0;
    char ch;

    while (1) {
        ch = getch();

        // Enter (finish)
        if (ch == '\r' || ch == '\n') {
            break;
        }

        // Backspace handling
        if (ch == '\b') {
            if (i > 0) {
                i--;
                printf("\b \b"); // remove one '*' from console
            }
            continue;
        }

        // Normal visible character range (avoid weird control chars)
        if (ch >= 32 && ch <= 126) {
            if (i < maxLen - 1) {
                pass[i++] = ch;
                printf("*");
            }
        }
    }

    pass[i] = '\0';
    printf("\n");
}

// ===================== LOGIN FUNCTION =====================
// role-specific login
int login(char *role) {
    char user[20], pass[20];
    int attempts = 0;

    while (attempts < 3) {
        printf("===== Login Screen (%s) =====\n", role);
        printf("Username: ");
        scanf("%s", user);

        printf("Password: ");
        readPassword(pass, sizeof(pass));   // <-- password will show as ****

        // Check role and credentials
        if (strcmp(role, "admin") == 0) {
            if (strcmp(user, "admin") == 0 && strcmp(pass, "1234") == 0) {
                printf("Admin login successful!\n");
                return 1;
            }
        } else if (strcmp(role, "staff") == 0) {
            if (strcmp(user, "staff") == 0 && strcmp(pass, "4321") == 0) {
                printf("Staff login successful!\n");
                return 1;
            }
        } else if (strcmp(role, "guest") == 0) {
            if (strcmp(user, "guest") == 0 && strcmp(pass, "0000") == 0) {
                printf("Guest login successful!\n");
                return 1;
            }
        } else {
            printf("Unknown role!\n");
            return 0;
        }

        attempts++;
        printf("Incorrect username or password! Attempts left: %d\n", 3 - attempts);
    }

    return 0; // failed login
}

// ===================== ADMIN MENU =====================
void adminMenu() {
    int choice;

    while (1) {
        printf("\n==== ADMIN MENU ====\n");
        printf("1. Add Student\n");
        printf("2. Display Students\n");
        printf("3. Search Student\n");
        printf("4. Update Student\n");
        printf("5. Delete Student\n");
        printf("6. Logout\n");
        printf("Enter Choice: ");

        if (scanf(" %d", &choice) != 1) {
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
            printf("Invalid choice!\n");
            continue;
        }

        switch (choice) {
            case 1: addStudent(); break;
            case 2: displayStudents(); break;
            case 3: searchStudent(); break;
            case 4: updateStudent(); break;
            case 5: deleteStudent(); break;
            case 6: return; // logout
            default: printf("Invalid choice!\n");
        }
    }
}

// ===================== STAFF MENU =====================
void staffMenu() {
    int choice;

    while (1) {
        printf("\n==== STAFF MENU ====\n");
        printf("1. Display Students\n");
        printf("2. Search Student\n");
        printf("3. Logout\n");
        printf("Enter Choice: ");

        if (scanf(" %d", &choice) != 1) {
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
            printf("Invalid choice!\n");
            continue;
        }

        switch (choice) {
            case 1: displayStudents(); break;
            case 2: searchStudent(); break;
            case 3: return; // logout
            default: printf("Invalid choice!\n");
        }
    }
}

// ===================== GUEST MENU =====================
void guestMenu() {
    printf("\n==== GUEST MENU ====\n");
    printf("Guests can only view student records.\n");
    displayStudents();
    printf("Press Enter to logout...\n");
    while (getchar() != '\n'); // consume leftover newline
    getchar(); // wait for enter
}

// ===================== HELPER FUNCTION TO CHECK DUPLICATE ROLL =====================
int rollExists(int roll) {
    struct Student s;
    FILE *fp = fopen(STUDENT_FILE, "r");
    if (!fp) return 0; // file doesn't exist yet so no duplicates

    while (fscanf(fp, "%d %49s %f", &s.roll, s.name, &s.marks) == 3) {
        if (s.roll == roll) {
            fclose(fp);
            return 1;  // found duplicate
        }
    }
    fclose(fp);
    return 0;  // no duplicate
}

// ===================== ADD STUDENT =====================
void addStudent() {
    struct Student s;

    printf("Enter Roll No: ");
    if (scanf(" %d", &s.roll) != 1) {
        printf("Invalid roll number.\n");
        return;
    }

    if (rollExists(s.roll)) {
        printf("Error: Roll number %d already exists! Please use a unique roll number.\n", s.roll);
        return;
    }

    FILE *fp = fopen(STUDENT_FILE, "a");
    if (!fp) {
        printf("Error opening student file for appending.\n");
        return;
    }

    printf("Enter Name: ");
    scanf("%s", s.name); // no spaces as you wanted
    printf("Enter Marks: ");
    if (scanf(" %f", &s.marks) != 1) {
        printf("Invalid marks.\n");
        fclose(fp);
        return;
    }

    fprintf(fp, "%d %s %.2f\n", s.roll, s.name, s.marks);
    fclose(fp);

    printf("Student Added Successfully!\n");
}

// ===================== DISPLAY STUDENTS =====================
void displayStudents() {
    struct Student s;
    FILE *fp = fopen(STUDENT_FILE, "r");

    if (!fp) {
        printf("No student records found!\n");
        return;
    }

    printf("\n==== Student Records ====\n");
    while (fscanf(fp, "%d %49s %f", &s.roll, s.name, &s.marks) == 3) {
        printf("Roll: %d | Name: %s | Marks: %.2f\n", s.roll, s.name, s.marks);
    }

    fclose(fp);
}

// ===================== SEARCH STUDENT =====================
void searchStudent() {
    int ch;
    printf("Search by:\n");
    printf("1. Roll Number\n");
    printf("2. Name\n");
    printf("Enter choice: ");

    if (scanf(" %d", &ch) != 1) {
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
        printf("Invalid choice!\n");
        return;
    }

    struct Student s;
    FILE *fp = fopen(STUDENT_FILE, "r");
    if (!fp) {
        printf("No student records found!\n");
        return;
    }

    if (ch == 1) {
        int r;
        printf("Enter Roll Number: ");
        if (scanf(" %d", &r) != 1) {
            printf("Invalid roll number.\n");
            fclose(fp);
            return;
        }

        int found = 0;
        while (fscanf(fp, "%d %49s %f", &s.roll, s.name, &s.marks) == 3) {
            if (s.roll == r) {
                printf("Record Found!\nRoll: %d | Name: %s | Marks: %.2f\n",
                       s.roll, s.name, s.marks);
                found = 1;
                break;
            }
        }
        if (!found) printf("No Student Found.\n");
    }
    else if (ch == 2) {
        char nm[50];
        printf("Enter Name: ");
        if (scanf(" %49s", nm) != 1) {
            printf("Invalid name input.\n");
            fclose(fp);
            return;
        }

        int found = 0;
        while (fscanf(fp, "%d %49s %f", &s.roll, s.name, &s.marks) == 3) {
            if (strcmp(s.name, nm) == 0) {
                printf("Record Found!\nRoll: %d | Name: %s | Marks: %.2f\n",
                       s.roll, s.name, s.marks);
                found = 1;
                break;
            }
        }
        if (!found) printf("No Student Found.\n");
    }
    else {
        printf("Invalid choice!\n");
    }

    fclose(fp);
}

// ===================== UPDATE STUDENT =====================
void updateStudent() {
    struct Student s;
    int r, found = 0;

    printf("Enter Roll No to Update: ");
    if (scanf(" %d", &r) != 1) {
        printf("Invalid roll number.\n");
        return;
    }

    FILE *fp = fopen(STUDENT_FILE, "r");
    FILE *temp = fopen("temp.txt", "w");

    if (!fp || !temp) {
        printf("File error.\n");
        if (fp) fclose(fp);
        if (temp) fclose(temp);
        return;
    }

    while (fscanf(fp, "%d %49s %f", &s.roll, s.name, &s.marks) == 3) {
        if (s.roll == r) {
            found = 1;
            printf("Enter New Name: ");
            scanf("%s", s.name);
            printf("Enter New Marks: ");
            scanf(" %f", &s.marks);
        }
        fprintf(temp, "%d %s %.2f\n", s.roll, s.name, s.marks);
    }

    fclose(fp);
    fclose(temp);

    remove(STUDENT_FILE);
    rename("temp.txt", STUDENT_FILE);

    if (found)
        printf("Record Updated Successfully!\n");
    else
        printf("Roll No Not Found!\n");
}

// ===================== DELETE STUDENT =====================
void deleteStudent() {
    struct Student s;
    int r, found = 0;

    printf("Enter Roll No to Delete: ");
    if (scanf(" %d", &r) != 1) {
        printf("Invalid roll number.\n");
        return;
    }

    FILE *fp = fopen(STUDENT_FILE, "r");
    FILE *temp = fopen("temp.txt", "w");

    if (!fp || !temp) {
        printf("File error.\n");
        if (fp) fclose(fp);
        if (temp) fclose(temp);
        return;
    }

    while (fscanf(fp, "%d %49s %f", &s.roll, s.name, &s.marks) == 3) {
        if (s.roll != r)
            fprintf(temp, "%d %s %.2f\n", s.roll, s.name, s.marks);
        else
            found = 1;
    }

    fclose(fp);
    fclose(temp);

    remove(STUDENT_FILE);
    rename("temp.txt", STUDENT_FILE);

    if (found)
        printf("Record Deleted Successfully!\n");
    else
        printf("Record Not Found!\n");
}
