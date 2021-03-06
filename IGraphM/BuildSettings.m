
(*
 * This file contains settings used to compile IGraphM from source.
 * The values below are for my own setup. If you wish to compile from source,
 * you will need to adjust them to match your own system.
 *
 * Note that the igraph library (e.g., -ligraph or libigraph.a) must be specified within this file.
 *)

Switch[$OperatingSystem,

  "MacOSX", (* Compilation settings for OS X *)
  $buildSettings = {
    (*"Compiler" -> CCompilerDriver`GenericCCompiler`GenericCCompiler,
    "CompilerInstallation" -> "/opt/local/bin",
    "CompilerName" -> "clang++-mp-7.0",
    "SystemCompileOptions" -> "-O3 -m64 -fPIC -framework Foundation -framework mathlink",*)

    (* IGraphM requires C++11. With OS X's default compiler this is supported 10.9 and up only,
       thus we need to override the default -mmacosx-version-min=10.6 option. LTemplate now
       does this automatically. *)
    "CompileOptions" -> {"-flto=thin" (* "-g -O0 -fno-omit-frame-pointer -fsanitize=address" *)},

    (* Statically link the igraph library *)
    "ExtraObjectFiles" -> {"$HOME/local/lib/libigraph.a", "$HOME/local/lib/libgmp.a", "$HOME/local/lib/libglpk.a", "$HOME/local/lib/libemon.a"},

    (* Set igraph location *)
    "IncludeDirectories" -> {"$HOME/local/include"},
    "LibraryDirectories" -> {"$HOME/local/lib"}
  },

  "Unix", (* Compilation settings for Linux *)
  $buildSettings = {
    "CompileOptions" -> {
      If[$SystemID =!= "Linux-ARM",
        (* Compile with -static-libgcc on non-RPi Linux for better compatibility with older distros *)
        (* Do not use -flto at this point when compiling on Ubuntu 16.04 as it leads to crashes when igraph returns an error *)
        Unevaluated@Sequence["-static-libgcc", "-D_GLIBCXX_USE_CXX11_ABI=0"(*, "-flto"*)],
        Unevaluated@Sequence[]
      ]
    },

    (* Statically link the igraph library *)
    "ExtraObjectFiles" -> {"$HOME/local/lib/libigraph.a", "$HOME/local/lib/libgmp.a", "$HOME/local/lib/libglpk.a", "$HOME/local/lib/libemon.a"},

    (* Set igraph location *)
    "IncludeDirectories" -> {"$HOME/local/include"},
    "LibraryDirectories" -> {"$HOME/local/lib"}
  },

  "Windows",
  $buildSettings = { 
    "CompileOptions" -> {"/EHsc", "/wd4244", "/DNOMINMAX"},
    "IncludeDirectories" -> {"C:\\msys64\\home\\%USERNAME%\\local\\include", "C:\\msys64\\home\\%USERNAME%\\lemon\\include"},
    "ExtraObjectFiles" -> {"C:\\msys64\\home\\%USERNAME%\\local\\lib\\libigraph.dll.a", "C:\\msys64\\home\\%USERNAME%\\lemon\\lib\\lemon.lib"}
  }
]
