auto add_include_dir(TString incdir) {
    std::cout << "Adding to include dirs: " << incdir << "\n";
    incdir.Prepend( ".include ");
    gROOT->ProcessLine( incdir.Data(), NULL );
}

void rootlogon()
{
    gSystem->Load("libspark");
    gSystem->Load("libspark_test_lib");

    gSystem->SetMakeSharedLib(
        "cd $BuildDir; echo \"Building in $(pwd)\";"
        "g++ -std=c++23 -c $Opt -pipe -Wall -fPIC -pthread $IncludePath $SourceFiles; "
        "g++ $ObjectFiles -shared -Wl,-soname,$LibName.so "
        "-O $LinkedLibs -o $SharedLib");
}
