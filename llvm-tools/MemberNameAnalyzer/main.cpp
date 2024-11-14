#include <clang/AST/AST.h>
#include <clang/AST/Decl.h>
#include <clang/AST/RecursiveASTVisitor.h>
#include <clang/ASTMatchers/ASTMatchFinder.h>
#include <clang/ASTMatchers/ASTMatchers.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Tooling/CommonOptionsParser.h>
#include <clang/Tooling/Tooling.h>

using namespace clang;
using namespace clang::tooling;
using namespace clang::ast_matchers;

class VariableNameCheckVisitor
    : public RecursiveASTVisitor<VariableNameCheckVisitor> {
 public:
  VariableNameCheckVisitor(ASTContext& context, std::string file)
      : context(context), file(std::move(file)) {}

  bool VisitVarDecl(const VarDecl* varDecl) {
    if (!varDecl->getName().starts_with("m_")) {
      auto& sm = context.getSourceManager();
      auto location = varDecl->getLocation();
      auto vdFile = sm.getFilename(location);
      if (vdFile == file && location.isValid()) {
        auto id = varDecl->getASTContext().getDiagnostics().getCustomDiagID(
            DiagnosticsEngine::Level::Error,
            "The variable '%0' has to start with 'm_', change it to 'm_%0'");
        auto& engine = varDecl->getASTContext().getDiagnostics();
        auto report = engine.Report(location, id);
        report.AddString(varDecl->getName());
        report << varDecl->getName();
      }
    }
    return true;
  }

 private:
  ASTContext& context;
  std::string file;
};

class VariableNameCheckConsumer : public ASTConsumer {
 public:
  VariableNameCheckConsumer(ASTContext& context, std::string file)
      : visitor(context, std::move(file)) {}

  void HandleTranslationUnit(clang::ASTContext& Context) override {
    visitor.TraverseDecl(Context.getTranslationUnitDecl());
  }

 private:
  VariableNameCheckVisitor visitor;
};

class VariableNameCheckAction : public ASTFrontendAction {
 protected:
  std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance& ci,
                                                 StringRef file) override {
    return std::make_unique<VariableNameCheckConsumer>(ci.getASTContext(),
                                                       file.str());
  }
};

int main(int argc, const char** argv) {
  static llvm::cl::OptionCategory category("Variable Name Checker");
  auto parser = CommonOptionsParser::create(argc, argv, category);
  auto tool = ClangTool(parser->getCompilations(), parser->getSourcePathList());
  tool.run(newFrontendActionFactory<VariableNameCheckAction>().get());
}
