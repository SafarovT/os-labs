#include "../../external/catch2/catch.hpp"
#include "../task2/BgThreadDispatcher.h"
#include <sstream>
#include <syncstream>
#include <chrono>
#include <thread>

using osync = std::osyncstream;

TEST_CASE("Simple task with wait")
{
	BgThreadDispatcher dispatcher;
	std::ostringstream output;

	dispatcher.Dispatch([&output] { osync(output) << "1"; });

	dispatcher.Wait();
	osync(output) << "!";

	REQUIRE(output.str() == "1!");
}

TEST_CASE("Operation before wait and task order")
{
	BgThreadDispatcher dispatcher;
	std::ostringstream output;

	dispatcher.Dispatch([&output] { osync(output) << "1"; });
	dispatcher.Dispatch([&output] { osync(output) << "2"; });
	osync(output) << "!";
	dispatcher.Wait();
	dispatcher.Dispatch([&output] { osync(output) << "3"; });
	dispatcher.Wait();

	bool isCorrectOutput = output.str() == "!123" || output.str() == "1!23" || output.str() == "12!3";
	REQUIRE(isCorrectOutput);
}

TEST_CASE("Test work without Wait and destructor")
{
	BgThreadDispatcher dispatcher;
	std::ostringstream output;
	dispatcher.Dispatch([&output] { osync(output) << "1"; });
	osync(output) << "!";
	
	bool isCorrectOutput = output.str() == "!" || output.str() == "1!" || output.str() == "!1";
	REQUIRE(isCorrectOutput);
}

TEST_CASE("Test work wait task to complete wait without .Wait()")
{
	BgThreadDispatcher dispatcher;
	std::ostringstream output;
	dispatcher.Dispatch([&output] { osync(output) << "1"; });
	osync(output) << "!";
	std::this_thread::sleep_for(std::chrono::seconds(1));
	bool isCorrectOutput = output.str() == "1!" || output.str() == "!1";
	REQUIRE(isCorrectOutput);
}

TEST_CASE("Test dispatch after")
{
	BgThreadDispatcher dispatcher;
	std::ostringstream output;
	dispatcher.Dispatch([&output] { osync(output) << "1"; });
	std::this_thread::sleep_for(std::chrono::seconds(1));
	dispatcher.Stop();
	dispatcher.Dispatch([&output] { osync(output) << "2"; });
	dispatcher.Wait();
	osync(output) << "!";

	REQUIRE(output.str() == "1!");
}

TEST_CASE("Wait without tasks")
{
	BgThreadDispatcher dispatcher;
	std::ostringstream output;
	dispatcher.Wait();
	osync(output) << "!";
	REQUIRE(output.str() == "!");
}

TEST_CASE("Order test")
{
	BgThreadDispatcher dispatcher;
	std::ostringstream output;
	auto second = std::chrono::seconds(1);
	dispatcher.Dispatch([&output] { osync(output) << "1"; });
	dispatcher.Dispatch([&output, second] { std::this_thread::sleep_for(second); osync(output) << "2"; });
	std::this_thread::sleep_for(std::chrono::milliseconds(500));
	REQUIRE(output.str() == "1");
	std::this_thread::sleep_for(second);
	REQUIRE(output.str() == "12");
}

TEST_CASE("Tasks after stop")
{
	BgThreadDispatcher dispatcher;
	std::ostringstream output;
	dispatcher.Stop();
	dispatcher.Dispatch([&output] { osync(output) << "1"; });
	dispatcher.Wait();
	osync(output) << "!";
	REQUIRE(output.str() == "!");
}