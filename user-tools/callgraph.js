#!/usr/bin/env node

const fs = require("fs");
const esprima = require("esprima");
const scopeAnalyzer = require("eslint-scope");

const args = process.argv.slice(2);

let excludes = new Set();
let filename = null;

for (let i = 0; i < args.length; i++) {
  if (args[i] === "--exclude" && args[i + 1]) {
    excludes = new Set(args[i + 1].split(","));
    i++;
  } else if (!args[i].startsWith("-")) {
    filename = args[i];
  }
}

if (!filename) {
  console.error("usage: node callgraph.js [--exclude fn1,fn2,...] file.js");
  process.exit(1);
}

const code = fs.readFileSync(filename, "utf8");
const ast = esprima.parseScript(code, { range: true });
const scopeManager = scopeAnalyzer.analyze(ast);

const functions = new Map();
const calls = new Map();

function functionName(node) {
  if (!node) return "<top>";
  if (node.id) return node.id.name;
  if (node.type === "FunctionExpression") return "<anon>";
  return "<anon>";
}

// collect functions
scopeManager.scopes.forEach(scope => {
  if (scope.block.type === "FunctionDeclaration") {
    functions.set(scope.block, functionName(scope.block));
    calls.set(functionName(scope.block), new Set());
  }
});

// walk AST manually
function walk(node, currentFunc) {
  if (!node || typeof node !== "object") return;

  if (node.type === "FunctionDeclaration") {
    currentFunc = functionName(node);
  }

  if (node.type === "CallExpression" && node.callee.type === "Identifier") {
    if (currentFunc) {
      calls.get(currentFunc)?.add(node.callee.name);
    }
  }

  for (const key in node) {
    const child = node[key];
    if (Array.isArray(child)) child.forEach(c => walk(c, currentFunc));
    else walk(child, currentFunc);
  }
}

walk(ast, null);

// output
console.log("digraph callgraph {");
console.log("  node [shape=box];");

for (const [fn, callees] of calls.entries()) {
  if (callees.size === 0) {
    console.log(`  "${fn}";`);
  } else {
    for (const callee of callees) {
      if (excludes.has(callee)) continue;
      console.log(`  "${fn}" -> "${callee}";`);
    }
    if (callees.size === 0) {
      console.log(`  "${fn}";`);
    }
  }
}

console.log("}");
