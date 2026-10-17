#include <iostream>
#include <unordered_map>
#include <stdexcept>
#include "ast.h"
#include "visitor.h"

using namespace std;

// Acepta un visitor sobre una expresión binaria
int BinaryExp::accept(Visitor* visitor) {
    return visitor->visit(this);
}

// Acepta un visitor sobre un literal entero
int NumberExp::accept(Visitor* visitor) {
    return visitor->visit(this);
}

// Acepta un visitor sobre una variable
int IdExp::accept(Visitor* visitor) {
    return visitor->visit(this);
}

// Acepta un visitor sobre una asignación
int AssignStmt::accept(Visitor* visitor) {
    visitor->visit(this);
    return 0;
}

// Acepta un visitor sobre un print
int PrintStmt::accept(Visitor* visitor) {
    visitor->visit(this);
    return 0;
}

// Acepta un visitor sobre un if
int IfStmt::accept(Visitor* visitor) {
    visitor->visit(this);
    return 0;
}

// Acepta un visitor sobre un while
int WhileStmt::accept(Visitor* visitor) {
    visitor->visit(this);
    return 0;
}

// Acepta un visitor sobre un do-while
int DoWhileStmt::accept(Visitor* visitor) {
    visitor->visit(this);
    return 0;
}

// Acepta un visitor sobre un break
int BreakStmt::accept(Visitor* visitor) {
    visitor->visit(this);
    return 0;
}

// Acepta un visitor sobre un ifexp
int IfExp::accept(Visitor* visitor) {
    return visitor->visit(this);
}

// Crea el generador de código, que escribe el ensamblador en out
GencodeVisitor::GencodeVisitor(ostream& out) : out(out) {}

// Genera el código de left op right y deja el resultado en %rax
int GencodeVisitor::visit(BinaryExp* exp) {
    exp->left->accept(this);
    out << "pushq %rax" << endl;
    exp->right->accept(this);
    out << "movq %rax, %rcx" << endl;
    out << "popq %rax" << endl;
    switch (exp->op) {
        case PLUS_OP:
            out << "addq %rcx, %rax" << endl;
            break;
        case MINUS_OP:
            out << "subq %rcx, %rax" << endl;
            break;
        case MUL_OP:
            out << "imulq %rcx, %rax" << endl;
            break;
        case DIV_OP:
            out << "cqto" << endl;              // extiende el signo de %rax a %rdx
            out << "idivq %rcx" << endl;        // %rax = %rdx:%rax / %rcx
            break;
        case POW_OP: {
            int n = etiquetas++;
            out << "movq %rax, %rsi" << endl;   // base
            out << "movq $1, %rax" << endl;     // resultado = 1
            out << "pow_" << n << ":" << endl;
            out << "cmpq $0, %rcx" << endl;     // ¿exponente <= 0?
            out << "jle endpow_" << n << endl;
            out << "imulq %rsi, %rax" << endl;  // resultado *= base
            out << "decq %rcx" << endl;         // exponente--
            out << "jmp pow_" << n << endl;
            out << "endpow_" << n << ":" << endl;
            break;
        }
        case LE_OP:
            out << "cmpq %rcx, %rax" << endl;
            out << "setle %al" << endl;
            out << "movzbq %al, %rax" << endl;
            break;

        default:
            break;
    }
    return 0;
}

// Carga el literal en %rax
int GencodeVisitor::visit(NumberExp* exp) {
    out << "movq $" << exp->value << ", %rax" << endl;
    return 0;
}

// Carga el valor de la variable desde la pila en %rax
int GencodeVisitor::visit(IdExp* exp) {
    out << "movq " << posicion[exp->value] * -8 << "(%rbp), %rax" << endl;
    return 0;
}

// Evalúa la expresión y la guarda en la posición de la variable (asignada la primera vez)
void GencodeVisitor::visit(AssignStmt* stm) {
    stm->e->accept(this);
    if (!posicion.count(stm->id)) {
        posicion[stm->id] = contador;
        contador++;
    }
    out << "movq %rax, " << posicion[stm->id] * -8 << "(%rbp)" << endl;
}

// Evalúa la expresión y la imprime con printf
void GencodeVisitor::visit(PrintStmt* stm) {
    stm->e->accept(this);
    out << "movq %rax, %rsi" << endl;
    out << "leaq print_fmt(%rip), %rdi" << endl;
    out << "movl $0, %eax" << endl;
    out << "call printf@PLT" << endl;
}

// if: si la condición vale 0 salta al else; si no, ejecuta el then y salta al final
void GencodeVisitor::visit(IfStmt* stm) {
    int n = etiquetas++;
    stm->condicion->accept(this);
    out << "cmpq $0, %rax" << endl;
    out << "je else_" << n << endl;
    for (Stmt* s : stm->thenList) {
        s->accept(this);
    }
    out << "jmp endif_" << n << endl;
    out << "else_" << n << ":" << endl;
    for (Stmt* s : stm->elseList) {
        s->accept(this);
    }
    out << "endif_" << n << ":" << endl;
}

// while: evalúa la condición en cada vuelta; si vale 0 sale, si no ejecuta el cuerpo y vuelve
void GencodeVisitor::visit(WhileStmt* stm) {
    int n = etiquetas++;
    salidas.push_back("endwhile_" + to_string(n));    // un break aquí adentro salta a endwhile_n
    out << "while_" << n << ":" << endl;
    stm->condicion->accept(this);
    out << "cmpq $0, %rax" << endl;
    out << "je endwhile_" << n << endl;
    for (Stmt* s : stm->body) {
        s->accept(this);
    }
    out << "jmp while_" << n << endl;
    out << "endwhile_" << n << ":" << endl;
    salidas.pop_back();
}

// do-while: primero el cuerpo, después la condición; si es distinta de 0 vuelve al inicio
void GencodeVisitor::visit(DoWhileStmt* stm) {
    int n = etiquetas++;
    salidas.push_back("enddowhile_" + to_string(n));  // un break aquí adentro salta a enddowhile_n
    out << "dowhile_" << n << ":" << endl;
    for (Stmt* s : stm->body) {
        s->accept(this);
    }
    stm->condicion->accept(this);
    out << "cmpq $0, %rax" << endl;
    out << "jne dowhile_" << n << endl;
    out << "enddowhile_" << n << ":" << endl;
    salidas.pop_back();
}

// break: salta a la salida del ciclo más interno (el tope de la pila salidas)
void GencodeVisitor::visit(BreakStmt* stm) {
    if (salidas.empty()) {
        throw runtime_error("Error semántico: break fuera de un ciclo");
    }
    out << "jmp " << salidas.back() << endl;
}

// ifexp(cond, e1, e2): deja en %rax e1 si cond != 0, si no e2 (solo evalúa una de las dos)
int GencodeVisitor::visit(IfExp* exp) {
    int n = etiquetas++;
    exp->condicion->accept(this);
    out << "cmpq $0, %rax" << endl;
    out << "je ifexp_else_" << n << endl;
    exp->e1->accept(this);
    out << "jmp ifexp_end_" << n << endl;
    out << "ifexp_else_" << n << ":" << endl;
    exp->e2->accept(this);
    out << "ifexp_end_" << n << ":" << endl;
    return 0;
}

// Genera el programa completo: datos, prólogo de main, sentencias y epílogo
void GencodeVisitor::gencode(Program* program) {
    out << ".data" << endl;
    out << "print_fmt:.string \"%ld\\n\" " << endl;
    out << ".text" << endl;
    out << ".globl main" << endl;
    out << "main:" << endl;
    out << "pushq %rbp" << endl;
    out << "movq %rsp, %rbp" << endl;
    out << "subq $64, %rsp" << endl;
    for (Stmt* s : program->slist) {
        s->accept(this);
    }
    out << "movq $0, %rax" << endl;
    out << "leave" << endl;
    out << "ret" << endl;
    out << ".section .note.GNU-stack,\"\",@progbits" << endl;
}
