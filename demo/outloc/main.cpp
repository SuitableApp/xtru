#include "pslib.h"

int main(int argc, char** argv)
{

    namespace sl = ps::lib::nsStreamLocator;
    try
    {
        const auto tn = boost::filesystem::path(argv[0]).replace_extension(".log").leaf();
        auto& trc = ps::lib::cTracer::get_mutable_instance();
        /// oRedirectTo() changes name of trc from default '_xtru_PID.log' to tn.
        trc.oRedirectTo(tn);
        sl::vInitialize(
            // destination for output.
            // Scheme name, a word front of "://", defines kind of redirector.
            "ipc_pipe://{E=HOME}/occi/demo/outloc/child"
            , ""    /// sOutput {O}
            , ""    /// sConnectTo {I}
            , {{"dat", "ctl", "clo", "blo"}}  // Types of target file generated when table is unloaded.
            , false // False means that the control file is outputted.
        );
        {
            sl::cStreamLocator locator("","");

            std::unique_ptr<std::ostream> os = locator.oOpen(sl::iExtData, "");
            for (int i = 1; i <= 12; i++)
            {
                *os << "Hello eveyone! N=" << i << std::endl;
            }
            *os << "quit" << std::endl;
        }
        sl::vInitialize(
            "ipc_pipe://zip /tmp/{E=HOSTNAME}_{O}_{T}_{P}_{C}_{A}_{I}_{X}_{D=yyyy'_'MM'_'dd}_{W=HH'_'mm'_'ss}.zip -v -"
            , ""    /// sOutput {O}
            , ""    /// sConnectTo {I}
            , {{"dat", "ctl", "clo", "blo"}}
            , false // False means that the control file is outputted.
        );
        {
            // ローカルプロセス ( 実行結果出力ストリーム指定 )
            sl::cStreamLocator locator("otp", "ctl");

            std::unique_ptr<std::ostream> os = locator.oOpen(sl::iExtData, "");

            for (int i = 1; i <= 12; i++)
            {
                *os << "Hello eveyone! N=" << i << std::endl;
            }
            *os << "test1" << std::endl;
        }
        sl::vInitialize(
            "ipc_pipe://zip /tmp/test2.zip -v -"
            , ""    /// sOutput {O}
            , ""    /// sConnectTo {I}
            , {{"dat", "ctl", "clo", "blo"}}
            , false // False means that the control file is outputted.
        );
        {
            // ローカルプロセス ( 実行結果出力先は標準 ( cout / cerr ) 出力 )
            sl::cStreamLocator locator("otp", "ctl");

            std::unique_ptr<std::ostream> os = locator.oOpen(sl::iExtData, "");

            *os << "test2" << std::endl;
        }
#if 0
#endif
        sl::vInitialize(
            "file://{E=HOME}/{O}/{T}.{X}"
            , "sa_home/output"    /// sOutput {O}
            , ""    /// sConnectTo {I}
            , {{"dat", "ctl", "clo", "blo"}}
            , false // False means that the control file is outputted.
        );
        {
            // ファイルシステム
            sl::cStreamLocator locator("otp", "ctl");

            std::unique_ptr<std::ostream> os = locator.oOpen(sl::iExtData, "");

            *os << "cFileSystem test." << std::endl;
        }
#if 0
        sl::vInitialize(
            "named_pipe:///tmp/test3", {{"dat", "ctl", "clo", "blo"}}, false);
        {
            // 名前付きパイプ
            sl::cStreamLocator locator("otp", "ctl");

            std::unique_ptr<std::ostream> os = locator.oOpen(sl::iExtData, "");

            *os << "test3" << std::endl;
        }
#endif
        return 0;
    }
    catch (const std::exception& ex)
    {
        std::cerr << ex.what();
        return -1;
    }
}

