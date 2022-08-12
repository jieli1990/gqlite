#pragma once
#include "Plan.h"
#include "base/lang/lang.h"
#include "base/lang/AST.h"
#include "base/Variant.h"
#include "gutil.h"
#include "json.hpp"
#include "Graph/GRAD.h"

struct GASTNode;
class GUpsetPlan: public GPlan {
public:
  GUpsetPlan(GVirtualNetwork* vn, GStorageEngine* store, GUpsetStmt* stmt);
  ~GUpsetPlan();

  virtual int prepare();
  virtual int execute(const std::function<ExecuteStatus(KeyType, const std::string& key, const std::string& value)>&);

private:
  /**
   * @brief JSONVisitor travels vertex/edge property and retrieve their infomation
   */
  struct JSONVisitor {
    nlohmann::json _jsonify;    /** read property as an json */
    std::string _key;           /** current read key */
    using var_t = Variant<std::string, double, int>;
    std::vector<var_t> _values; /** current read value in _key */
    //GUpsetPlan& _plan;
    JSONVisitor(GUpsetPlan& plan) {}
    
    VisitFlow apply(GASTNode* stmt, std::list<NodeType>& path) {
      return VisitFlow::Children;
    }
    VisitFlow apply(GUpsetStmt* stmt, std::list<NodeType>& path) {
      return VisitFlow::Children;
    }
    VisitFlow apply(GQueryStmt* stmt, std::list<NodeType>& path) {
      return VisitFlow::Return;
    }
    VisitFlow apply(GGQLExpression* stmt, std::list<NodeType>& path) {
      return VisitFlow::Children;
    }
    VisitFlow apply(GProperty* stmt, std::list<NodeType>& path) {
      add();
      _key = stmt->key();
      _values.clear();
      return VisitFlow::Children;
    }
    VisitFlow apply(GVertexDeclaration* stmt, std::list<NodeType>& path) {
      return VisitFlow::Children;
    }
    VisitFlow apply(GCreateStmt* stmt, std::list<NodeType>& path) {
      return VisitFlow::Return;
    }
    VisitFlow apply(GDumpStmt* stmt, std::list<NodeType>& path) {
      return VisitFlow::SkipCurrent;
    }
    VisitFlow apply(GLiteral* stmt, std::list<NodeType>& path) {
      switch (stmt->kind()) {
      case AttributeKind::Binary:
        // TODO:
        break;
      case AttributeKind::Datetime:
        // TODO:
        break;
      case AttributeKind::Integer:
        _values.push_back(atoi(stmt->raw().c_str()));
        break;
      case AttributeKind::Number:
        _values.push_back(atof(stmt->raw().c_str()));
        break;
      case AttributeKind::String:
        _values.push_back(stmt->raw());
        break;
      default:
        break;
      }
      return VisitFlow::Children;
    }
    VisitFlow apply(GArrayExpression* stmt, std::list<NodeType>& path) {
      return VisitFlow::Children;
    }
    VisitFlow apply(GEdgeDeclaration* stmt, std::list<NodeType>& path) {
      return VisitFlow::Children;
    }
    VisitFlow apply(GDropStmt* stmt, std::list<NodeType>& path) {
      return VisitFlow::Return;
    }
    VisitFlow apply(GRemoveStmt* stmt, std::list<NodeType>& path) {
      return VisitFlow::Return;
    }

    void add() {
      if (!_key.empty()) {
        if (_values.size() == 1) {
          _values[0].visit(
            [&](int value) {
              _jsonify[_key] = value;
            },
            [&](double value) {
              _jsonify[_key] = value;
            },
            [&](std::string value) {
              _jsonify[_key] = value;
            });
        }
        else {
          for (auto& item : _values)
          {
            item.visit(
              [&](int value) {
                _jsonify[_key].push_back(value);
              },
              [&](double value) {
                _jsonify[_key].push_back(value);
              },
              [&](std::string value) {
                _jsonify[_key].emplace_back(value);
              });
          }
        }
      }
      else {
        if (_values.size() == 1) {
          _values[0].visit(
            [&](int value) {
              _jsonify = value;
            },
            [&](double value) {
              _jsonify = value;
            },
              [&](std::string value) {
              _jsonify = value;
            });
        }
      }
    }
  };

  struct UpsetVisitor {
    GUpsetPlan& _plan;

    UpsetVisitor(GUpsetPlan& plan): _plan(plan) {}
    VisitFlow apply(GASTNode* stmt, std::list<NodeType>& path) {
      return VisitFlow::Children;
    }
    VisitFlow apply(GUpsetStmt* stmt, std::list<NodeType>& path) {
      return VisitFlow::Children;
    }
    VisitFlow apply(GQueryStmt* stmt, std::list<NodeType>& path) {
      return VisitFlow::Return;
    }
    VisitFlow apply(GGQLExpression* stmt, std::list<NodeType>& path) {
      return VisitFlow::Children;
    }
    VisitFlow apply(GProperty* stmt, std::list<NodeType>& path) {
      return VisitFlow::Children;
    }
    VisitFlow apply(GDumpStmt* stmt, std::list<NodeType>& path) {
      return VisitFlow::SkipCurrent;
    }
    VisitFlow apply(GVertexDeclaration* stmt, std::list<NodeType>& path);
    VisitFlow apply(GCreateStmt* stmt, std::list<NodeType>& path) {
      return VisitFlow::Return;
    }
    VisitFlow apply(GLiteral* stmt, std::list<NodeType>& path) {
      return VisitFlow::Children;
    }
    VisitFlow apply(GArrayExpression* stmt, std::list<NodeType>& path) {
      for (auto itr = stmt->begin(), end = stmt->end(); itr != end; ++itr) {
        VisitFlow vf = accept(*itr, *this, path);
        if (vf == VisitFlow::Children || vf == VisitFlow::SkipCurrent) continue;
        return vf;
      }
      return VisitFlow::Children;
    }
    VisitFlow apply(GEdgeDeclaration* stmt, std::list<NodeType>& path);
    VisitFlow apply(GDropStmt* stmt, std::list<NodeType>& path) {
      return VisitFlow::Return;
    }
    VisitFlow apply(GRemoveStmt* stmt, std::list<NodeType>& path) {
      return VisitFlow::Return;
    }
    key_t getLiteral(GASTNode* node);
  };

  friend struct UpsetVisitor;

private:
  bool upsetVertex();
  bool upsetEdge();
private:
  bool _vertex;       /**< true if upset target is vertex, else is edge */
  std::string _class;
  std::map<key_t, std::string> _vertexes;
  std::map<gql::edge_id, std::string> _edges;
};