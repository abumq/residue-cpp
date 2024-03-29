/*
 * This is a sample app that uses Residue client library for C++
 *
 * Revision: 1.0
 *
 * https://github.com/abumq/residue/tree/master/lib
 */

#include <unistd.h>
#include <cstring> // strcmp
#include <vector> // to demonstrate STL logging
#include "log.h" // also includes Residue

void displayErrors()
{
    if (!Residue::errors().empty()) {
        std::cout << "Last 10 Errors from the server (latest is last):" << std::endl;
        for (std::string e : Residue::errors()) {
            std::cout << e << std::endl;
        }
    } else {
        std::cout << "No errors recorded!" << std::endl;
    }
}

int main(int argc, char* argv[]) {

    std::cout << Residue::info() << std::endl;

    // Residue::setInternalLoggingLevel(Residue::InternalLoggingLevel::crazy);

#if 0 // minimal sample with unknown logger
    try {
        Residue::connect("residue-server", Residue::DEFAULT_PORT);
    } catch (const ResidueException& e) {
        std::cout << "EXCEPTION: " << e.what() << std::endl;
        return 1;
    }

    if (Residue::connected()) {
        std::cout << "Connected!" << std::endl;
        std::cout << "Server version: " << Residue::instance().serverVersion() << std::endl;
    } else {
        std::cout << "Failed!" << std::endl;
        displayErrors();
    }
    LOG(DEBUG) << "Debug 1";
    LOG(ERROR) << "Error 1";
    CLOG(INFO, "sample-app-unknown-logger") << "user is zooming";
    Residue::disconnect();
    return 0;
#endif

    // Residue::enablePlainRequest();
    // THIS IS OPTIONAL IF YOU WANT TO READ VERBOSE LOG LEVEL (i.e, -v)
    // SEE https://github.com/abumq/easyloggingpp#verbose-logging
    Residue::setApplicationArgs(argc, argv);

#if 0 // Configure via config file
    std::string clientConfigFile = "/Users/abumusamq/Dev/residue-cpp/samples/client.conf.json";
    Residue::loadConfiguration(clientConfigFile);
#else

    // review the privateKeyFile location (try running this sample from root)
    std::string keyBase = "/Users/abumusamq/Dev/residue/tools/netcat-client/";

    // Manually set configurations via API
#   if 1 // enable known client
    // OPTIONAL:
    // If you are provided some details about client ID from server and you have uploaded
    // RSA public key to the server then you can let server known about this and you may get
    // other benefits (depending on your host providing)
    //
    //   - https://github.com/abumq/residue/blob/master/docs/CONFIGURATION.md#allow_unknown_clients
    //   - https://github.com/abumq/residue/blob/master/docs/CONFIGURATION.md#known_clients
    //
    std::string privateKeyFile = keyBase + "client-256-private.pem";
    std::fstream fs(privateKeyFile, std::ios::in);
    if (!fs.is_open()) {
        std::cerr << "File not readable: " << privateKeyFile << std::endl;
        return 1;
    }
    std::string privateKey = std::string(std::istreambuf_iterator<char>(fs), std::istreambuf_iterator<char>());
    // Remember, this client ID should be correct otherwise you will get UNKNOWN CLIENT error
    // also this private key should be corresponding to public key on the server or you will not able to
    // decrypt response from the server
    Residue::setKnownClient("muflihun00102030", privateKey);
#   endif


#   if 0 // Enable server key
    // Notice, the bigger the server key, the slower server will respond (but it's more secure)
    std::string serverPublicKeyFile = keyBase + "server-1024-public.pem";
    std::fstream fs2(serverPublicKeyFile, std::ios::in);
    if (!fs2.is_open()) {
        std::cerr << "File not readable: " << serverPublicKeyFile << std::endl;
        return 1;
    }
    std::string serverPublicKey = std::string(std::istreambuf_iterator<char>(fs2), std::istreambuf_iterator<char>());

    Residue::setServerPublicKey(serverPublicKey);
#   endif

    Residue::setThreadName("Main Thread");
#endif

    try {
        Residue::connect("residue-server", Residue::DEFAULT_PORT);
    } catch (const ResidueException& e) {
        std::cout << "EXCEPTION: " << e.what() << std::endl;
        return 1;
    }

    if (Residue::connected()) {
        std::cout << "Connected! [Client ID: " << Residue::clientId() << "]" << std::endl;
    } else {
        std::cout << "Failed to connect to residue server. Unknown error" << std::endl;
        displayErrors();
        return 1;
    }

#if 0 // Test crash handler

    Residue::enableCrashHandler();

    std::string* s = new std::string();
    delete s;
    s = nullptr;
    s->clear(); // Crash!
#endif

    // In perfect world, you should be connected to the server now.
    // If anywhere in the program you wish to disconnect and log locally
    // simply use Residue::disconnect() and after that you can
    // call Residue::connect()

#if 1
    //
    // Optional: Providing "--bulk" option enables bulk dispatch for residue which means each log request
    // will be sent to the server in batch of bulk size. See following links for details
    //   - https://github.com/abumq/residue/blob/master/docs/CONFIGURATION.md#allow_bulk_log_request
    //   - https://abumq.github.io/residue/class_residue.html#a3bc1c73a82d964ec40600cb72f99645f
    //   - https://abumq.github.io/residue/class_residue.html#a4c6d59a5076991c70a0e71b0abd9935d
    //
    // If you wish to see the speed difference between bulk and single requests you can run following two commands.
    // These commands will run this sample 10 times and give you average time it took to finish.
    //     time for i in {1..10}; do ./residue-sample-app; done | grep real
    //     time for i in {1..10}; do ./residue-sample-app --bulk; done | grep real
    //
    // When we ran these two commands (OS X El Capitan 10.11.6, 3.1 GHz Intel Core i7, 16 GB Ram) we got following result:
    // Residue server was running on the same machine as this sample app with:
    //
    //     - PRODUCTION flag
    //     - Verbose logging turned off (no "-v" option)
    //     - dispatch_delay = 1
    //     - token_age = 3600
    //     - key_size = 2048
    //
    //    |  Type        |    Time Taken  |    Test #    |
    //    |==============|================|==============|
    //    |  Single      |    3.975s      |      1       |
    //    |  Single      |    3.727s      |      2       |
    //    |  Single      |    3.637s      |      3       |
    //    |  BulkSize=2  |    3.316s      |      1       |
    //    |  BulkSize=2  |    3.220s      |      2       |
    //    |  BulkSize=2  |    3.255s      |      3       |
    //    |  BulkSize=3  |    2.749s      |      1       |
    //    |  BulkSize=3  |    2.954s      |      2       |
    //    |  BulkSize=3  |    3.108s      |      3       |
    //    |  BulkSize=4  |    2.798s      |      1       |
    //    |  BulkSize=4  |    2.838s      |      2       |
    //    |  BulkSize=4  |    3.171s      |      3       |
    //    |  BulkSize=5  |    2.880s      |      1       |
    //    |  BulkSize=5  |    2.563s      |      2       |
    //    |  BulkSize=5  |    2.926s      |      3       |
    //
    // With known client [useKnownClient = true] (i.e, no overhead of key generation on the fly)
    //
    //    |  Type        |    Time Taken  |    Test #    |
    //    |==============|================|==============|
    //    |  Single      |    2.958s      |      1       |
    //    |  Single      |    3.015s      |      2       |
    //    |  Single      |    3.019s      |      3       |
    //    |  BulkSize=2  |    2.257s      |      1       |
    //    |  BulkSize=2  |    2.316s      |      2       |
    //    |  BulkSize=2  |    2.377s      |      3       |
    //    |  BulkSize=3  |    2.510s      |      1       |
    //    |  BulkSize=3  |    2.423s      |      2       |
    //    |  BulkSize=3  |    2.729s      |      3       |
    //    |  BulkSize=4  |    2.262s      |      1       |
    //    |  BulkSize=4  |    2.200s      |      2       |
    //    |  BulkSize=4  |    2.238s      |      3       |
    //    |  BulkSize=5  |    1.797s      |      1       |
    //    |  BulkSize=5  |    1.838s      |      2       |
    //    |  BulkSize=5  |    1.738s      |      3       |
    //
    // You can see with same program with different bulk sizes has significant effect on running time.
    //
    Residue::disableBulkDispatch();
    Residue::disableAutoBulkParams();
    if (argc > 1 && strcmp(argv[1], "--bulk") == 0) {
        std::cout << "Bulk requests!" << std::endl;
        try {
            Residue::setBulkSize(50);
            Residue::enableBulkDispatch();
        } catch (const ResidueException& e) {
            std::cerr << "ERROR: " << e.what() << std::endl;
            Residue::disableBulkDispatch();
        }
    }
#endif

#if 0 // enable utc and time offset
    // Optional! Enable UTC time for universal
    // software. Or you can use disableUtc() after this to use
    // local time.
    Residue::enableUtc();
    // You can set time offset for DST or other reasons if needed
    Residue::setTimeOffset(-3600);
#endif

    // Below code is only for benchmark test
#if 0
    auto start = std::chrono::high_resolution_clock::now();
    for (unsigned long i = 1; i <= 1000000; ++i) {
        LOG(INFO) << "Test " << i;
    }
    auto end = std::chrono::high_resolution_clock::now();

    auto result = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    std::cout << "Disconnecting... It took " << result << " ms to finish adding to the queue" << std::endl;
    Residue::disconnect();
    return 0;
#endif

    // Here we are using Easylogging++ macros to send log requests to Residue
    // Nothing will be logged locally as connecting (Residue::connect) will "uninstall" the default
    // log dispatcher and installs it's own dispatcher. If you want to know more about this technique
    // see https://github.com/abumq/easyloggingpp#log-dispatch-callback
    LOG(INFO) << "Test info";
    LOG(DEBUG) << "Test debug";
    LOG(ERROR) << "Test error";
    VLOG(1) << "Verbose log level-1";

    CLOG(INFO, "default") << "Test using default logger";
    CLOG(INFO, "sample-app-unknown-logger") << "Test using unknown logger";

#if 1 // more logging
    // You can reconnect whenever you like
    // Residue::reconnect();

    LOG(INFO) << "Test2";

    LOG(INFO) << "Test3";
    LOG(INFO) << "Test4";

    LOG(INFO) << "Test5";

    // You can even log STL data structures. See https://github.com/abumq/easyloggingpp#stl-logging for details
    std::vector<std::string> list { "first", "second", "third" };
    LOG(INFO) << list;

    std::vector<std::thread> threads;
    {
        // You can even run peformance tracking and on dispatching this is going to send it to residue
        // For more information about performance tracking please see https://github.com/abumq/easyloggingpp#performance-tracking
        TIMED_SCOPE(t, "Create Threads");
        auto create = [](int i) {
            LOG(INFO) << "Starting thread " << i;

            Residue::setThreadName("Thread " + std::to_string(i));

            TIMED_SCOPE(t2, "Sent Log Messages from Thread " + std::to_string(i));
            for (int j = 1; j <= 300; ++j) {
                LOG(INFO) << "Thread " << i << ", j " << j;
            }
        };

        for (int i = 1; i <= 5; ++i) {
            threads.push_back(std::thread([&]() {
                create(i);
            }));
        }
    }

    for (int i = 1; i <= 3; ++i) {
        LOG(INFO) << "Loop test " << i;
    }

    LOG(DEBUG) << "Debug 1";

    LOG(INFO) << "Test6";
    LOG(INFO) << "Test7";

    for (int i = 8; i <= 30; ++i) {
        LOG(INFO) << "Test " << i;
    }

    std::cout << "Join threads..." << std::endl;

    for (auto& t : threads) {
        // We wait for all the threads to finish
        t.join();
    }

#endif

    std::cout << "Disconnecting..." << std::endl;

    // This is important, we are manually disconnecting from the residue server
    // The program will not finish until all the log messages are sent to the server
    Residue::disconnect();

    // Once disconnected default log dispatcher for Easylogging++ is installed back
    // hence you will see normal log messages stored locally. This is only for demonstration purpose
    // you should not log anything after disconnect.
    LOG(INFO) << "After disconnected!";

    std::cout << "Ending\n";

    return 0;
}
