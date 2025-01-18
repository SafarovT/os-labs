#pragma once
#include "Line.h"
#include <boost/asio.hpp>
#include <thread>
#include <mutex>
#include <iostream>

using boost::asio::ip::tcp;

std::vector<Line> serverLines;
std::vector<std::shared_ptr<tcp::socket>> serverClients;
std::mutex serverMutex;

const std::vector<std::pair<std::string, sf::Color>> WB_COLOR_MENU =
{
    {"Black", sf::Color::Black},
    {"Red", sf::Color::Red},
    {"Green", sf::Color::Green},
    {"Blue", sf::Color::Blue}
};

void RunServer(int port)
{
    boost::asio::io_context ioContext;
    tcp::acceptor acceptor(ioContext, tcp::endpoint(tcp::v4(), port));

    sf::RenderWindow window(sf::VideoMode(800, 600), "Whiteboard Server");

    std::thread([&acceptor]()
        {
            while (true)
            {
                auto socket = std::make_shared<tcp::socket>(acceptor.get_executor());
                acceptor.accept(*socket);

                std::lock_guard<std::mutex> lock(serverMutex);
                serverClients.push_back(socket);
            }
        }).detach();

        Line currentLine;
        bool drawing = false;
        sf::Color currentColor = sf::Color::Black;
        const size_t maxPoints = 750;

        while (window.isOpen())
        {
            sf::Event event;
            while (window.pollEvent(event))
            {
                if (event.type == sf::Event::Closed)
                {
                    window.close();
                }
                else if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
                {
                    for (size_t i = 0; i < WB_COLOR_MENU.size(); ++i)
                    {
                        sf::FloatRect colorButtonRect(10, 10 + i * 40, 50, 30);
                        if (colorButtonRect.contains(event.mouseButton.x, event.mouseButton.y))
                        {
                            currentColor = WB_COLOR_MENU[i].second;
                            std::cout << "Selected color: " << WB_COLOR_MENU[i].first << std::endl;
                            break;
                        }
                    }

                    if (!drawing)
                    {
                        drawing = true;
                        currentLine.points.clear();
                        currentLine.color = currentColor;
                        currentLine.points.push_back(window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y)));
                    }
                }
                else if (event.type == sf::Event::MouseMoved && drawing)
                {
                    currentLine.points.push_back(window.mapPixelToCoords(sf::Vector2i(event.mouseMove.x, event.mouseMove.y)));

                    if (currentLine.points.size() >= maxPoints)
                    {
                        drawing = false;

                        std::lock_guard<std::mutex> lock(serverMutex);
                        serverLines.push_back(currentLine);

                        currentLine.points.clear();

                        for (auto& client : serverClients)
                        {
                            try
                            {
                                size_t pointCount = 0;
                                boost::asio::write(*client, boost::asio::buffer(&pointCount, sizeof(pointCount)));
                            }
                            catch (std::exception& e)
                            {
                                std::cerr << "Exception while sending data to client: " << e.what() << std::endl;
                            }
                        }
                    }
                    else
                    {
                        std::lock_guard<std::mutex> lock(serverMutex);
                        for (auto& client : serverClients)
                        {
                            try
                            {
                                if (currentLine.points.size())
                                {
                                    size_t pointCount = 1;
                                    boost::asio::write(*client, boost::asio::buffer(&pointCount, sizeof(pointCount)));

                                    boost::asio::write(*client, boost::asio::buffer(&currentLine.points.back(), sizeof(sf::Vector2f)));

                                    boost::asio::write(*client, boost::asio::buffer(&currentLine.color, sizeof(currentLine.color)));
                                }
                            }
                            catch (std::exception& e)
                            {
                                std::cerr << "Exception while sending data to client: " << e.what() << std::endl;
                            }
                        }
                    }
                }
                else if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left)
                {
                    drawing = false;

                    currentLine.points.push_back(window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y)));

                    std::lock_guard<std::mutex> lock(serverMutex);
                    serverLines.push_back(currentLine);

                    currentLine.points.clear();

                    for (auto& client : serverClients)
                    {
                        try
                        {
                            size_t pointCount = 0;
                            boost::asio::write(*client, boost::asio::buffer(&pointCount, sizeof(pointCount)));
                        }
                        catch (std::exception& e)
                        {
                            std::cerr << "Exception while sending data to client: " << e.what() << std::endl;
                        }
                    }
                }
            }

            window.clear(sf::Color::White);

            std::lock_guard<std::mutex> lock(serverMutex);
            for (const auto& line : serverLines)
            {
                sf::VertexArray va(sf::LineStrip, line.points.size());
                for (size_t i = 0; i < line.points.size(); ++i)
                {
                    va[i].position = line.points[i];
                    va[i].color = line.color;
                }
                window.draw(va);
            }

            if (!currentLine.points.empty())
            {
                sf::VertexArray va(sf::LineStrip, currentLine.points.size());
                for (size_t i = 0; i < currentLine.points.size(); ++i)
                {
                    va[i].position = currentLine.points[i];
                    va[i].color = currentLine.color;
                }
                window.draw(va);
            }

            for (size_t i = 0; i < WB_COLOR_MENU.size(); ++i)
            {
                sf::RectangleShape colorButton(sf::Vector2f(50, 30));
                colorButton.setPosition(10, 10 + i * 40);
                colorButton.setFillColor(WB_COLOR_MENU[i].second);
                window.draw(colorButton);
            }

            window.display();
        }
}