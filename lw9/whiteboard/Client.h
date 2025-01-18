#pragma once
#include "Line.h"
#include <boost/asio.hpp>
#include <iostream>

namespace
{
    void DrawLines(sf::RenderWindow& window, std::vector<Line>& lines)
    {
        window.clear(sf::Color::White);

        for (auto& line : lines)
        {
            auto lineSize = line.points.size();
            sf::VertexArray va(sf::LineStrip, lineSize);
            for (size_t i = 0; i < lineSize; ++i)
            {
                va[i].position = line.points[i];
                va[i].color = line.color;
            }
            window.draw(va);
        }

        window.display();
    }
}

void RunClient(const std::string& address, int port)
{
    boost::asio::io_context ioContext;
    tcp::socket socket(ioContext);
    socket.connect(tcp::endpoint(boost::asio::ip::make_address(address), port));

    sf::RenderWindow window(sf::VideoMode(800, 600), "Whiteboard Client");

    std::vector<Line> clientLines;
    clientLines.push_back(Line());
    std::mutex clientMutex;

    std::jthread([&socket, &clientLines, &clientMutex]()
        {
            try
            {
                while (true)
                {
                    size_t pointCount;
                    boost::asio::read(socket, boost::asio::buffer(&pointCount, sizeof(pointCount)));

                    if (pointCount == 0)
                    {
                        clientLines.push_back(Line());
                    }
                    else
                    {
                        sf::Vector2f point;
                        boost::asio::read(socket, boost::asio::buffer(&point, sizeof(point)));

                        sf::Color color;
                        boost::asio::read(socket, boost::asio::buffer(&color, sizeof(color)));

                        clientLines[clientLines.size() - 1].points.push_back(point);
                        clientLines[clientLines.size() - 1].color = color;
                    }
                }
            }
            catch (std::exception& e)
            {
                std::cerr << "Exception in client read thread: " << e.what() << std::endl;
            }
        }).detach();

        while (window.isOpen())
        {
            sf::Event event;
            while (window.pollEvent(event))
            {
                if (event.type == sf::Event::Closed)
                {
                    window.close();
                }
            }

            DrawLines(window, clientLines);
        }
}