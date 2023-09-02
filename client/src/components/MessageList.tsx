// import React, { useEffect, useState } from 'react';
// import messagesData from './messages.json';

// type MessageType = {
//   id: string;
//   text: string;
// };

// const MessagesList: React.FC = () => {
//   const [messages, setMessages] = useState<MessageType[]>([]);

//   useEffect(() => {
//     const fetchData = async () => {
//       try {
//         setMessages(Object.values(messagesData));
//       } catch (error) {
//         console.error('Не удалось получить сообщения:', error);
//       }
//     };

//     fetchData();
//   }, []);

//   return (
//     <div>
//       <h1>Список сообщений</h1>
//       <ul>
//         {messages.map((message) => (
//           <li key={message.id}>{message.text}</li>
//         ))}
//       </ul>
//     </div>
//   );
// };

// export default MessagesList;
import React, { useState, useEffect } from "react";
import axios from "axios";

type Message = {
  id: string;
  text: string;
};

const MessagesList = () => {
  const [messages, setMessages] = useState<Message[]>();

  useEffect(() => {
    axios
      .get("http://localhost:5000/messages")
      .then((res) => setMessages(res.data))
      .catch((err) => console.log(err));
  }, []);

  return messages ? (
    <div>
      {Object.entries(messages).map(([key, message]) => (
        <div key={key}>
          <p>ID: {message.id}</p>
          <p>Text: {message.text}</p>
        </div>
      ))}
    </div>
  ) : (
    <div>Loading...</div>
  );
};

export default MessagesList;