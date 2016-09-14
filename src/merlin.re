open Core.Std;

open Jenga_lib.Api;

open Utils;

open NpmDep;

/* See comment in the `sprintf` */
let dotMerlinScheme isTopLevelLib::isTopLevelLib libName::libName dir::dir => {
  let dotMerlinPath = rel dir::dir ".merlin";
  let saveMerlinAction thirdPartyOcamlfindLibNames => {
    let dotMerlinContent =
      Printf.sprintf
        {|# This file is autogenerated for
# [Merlin](https://github.com/the-lambda-church/merlin), a static analyser for
# OCaml that provides autocompletion, jump-to-location, recoverable syntax
# errors, type errors detection, etc., that your editor can use. To activate it,
# one usually provides a .merlin file at the Path.the_root of a project, describing where
# the sources and artifacts are. Since we dictated the project structure, we can
# auto generate .merlin files!

# S is the merlin flag for source files
%s

# Include all the third-party sources too. You might notice that we've put a
# .merlin into each node_modules package. This is subtle; in short, it's to make
# jump-to-location work correctly in conjunction with our build & namespacing
# setup, when you jump into a third-party file.
S %s

# B stands for build (artifacts). We generate ours into _build
B %s

# PKG lists packages found through ocamlfind (aka findlib), a tool for finding
# the location of third-party dependencies. For us, most of our third-party deps
# reside in `node_modules/` (made visible to Merlin through the S command
# above); this PKG command is for discovering the opam/ocamlfind packages.
PKG %s

# FLG is the set of flags to pass to Merlin, as if it used ocamlc to compile and
# understand our sources. You don't have to understand what these flags are for
# now; but if you're curious, go check the rebel.ml that generated this
# .merlin at https://github.com/reasonml/rebel
FLG -w -30 -w -40 -open %s
|}
        (isTopLevelLib ? "S src" : "")
        (Path.reach_from dir::dir (rel dir::nodeModulesRoot "**/src"))
        (Path.reach_from dir::dir (rel dir::buildDirRoot "*"))
        (thirdPartyOcamlfindLibNames |> List.map f::tsl |> String.concat sep::" ")
        (tsm (libToModule libName));
    Action.save dotMerlinContent target::dotMerlinPath
  };
  Scheme.rules [
    Rule.create
      targets::[dotMerlinPath]
      (Dep.map (Dep.return (getThirdPartyOcamlfindLibs libDir::dir)) saveMerlinAction)
  ]
};
