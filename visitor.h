#ifndef VISITOR_H
#define VISITOR_H

#include <list>
#include <vector>
#include <string>
#include "ast.h"

class BinaryExp;
class NumberExp;
class IdExp;
class AssignStmt;
class PrintStmt;
class IfStmt;
class WhileStmt;
class DoWhileStmt;
class BreakStmt;
class IfExp;
class Program;

// Interfaz Visitor: una operación visit por cada nodo del AST
class Visitor {
public:
    virtual int visit(BinaryExp* exp) = 0;
    virtual int visit(NumberExp* exp) = 0;
    virtual int visit(IdExp* exp) = 0;
    virtual void visit(AssignStmt* stm) = 0;
    virtual void visit(PrintStmt* stm) = 0;
    virtual void visit(IfStmt* stm) = 0;
    virtual void visit(WhileStmt* stm) = 0;
    virtual void visit(DoWhileStmt* stm) = 0;
    virtual void visit(BreakStmt* stm) = 0;
    virtual int visit(IfExp* exp) = 0;
};

// Visitor que genera código ensamblador x86-64 a partir del AST
class GencodeVisitor : public Visitor {
public:
    ostream& out;
    int contador = 1;
    unordered_map<string, int> posicion;
    int etiquetas = 0;          // contador para que cada etiqueta sea única
    vector<string> salidas;     // pila: etiqueta de salida de cada ciclo abierto (para break)
    GencodeVisitor(ostream& out);
    void gencode(Program* program);
    int visit(BinaryExp* exp) override;
    int visit(NumberExp* exp) override;
    int visit(IdExp* exp) override;
    void visit(AssignStmt* stm) override;
    void visit(PrintStmt* stm) override;
    void visit(IfStmt* stm) override;
    void visit(WhileStmt* stm) override;
    void visit(DoWhileStmt* stm) override;
    void visit(BreakStmt* stm) override;
    int visit(IfExp* exp) override;
};

#endif // VISITOR_H
