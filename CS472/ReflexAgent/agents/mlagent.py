import torch
import torch.nn as nn
import torch.optim as optim
import numpy as np
import random
from sklearn.preprocessing import MinMaxScaler

class RobotNN(nn.Module):
    def __init__(self, input_size, output_size):
        super().__init__()
        self.model = nn.Sequential(
            nn.Linear(input_size, 128),
            nn.ReLU(),
            nn.BatchNorm1d(128),
            nn.Linear(128, 64),
            nn.ReLU(),
            nn.BatchNorm1d(64),
            nn.Linear(64, 32),
            nn.ReLU(),
            nn.Linear(32, output_size)
        )

    def forward(self, x):
        return self.model(x)

class MLAgent:
    def __init__(self, input_size, output_size):
        self.model = RobotNN(input_size, output_size)
        self.scaler = MinMaxScaler()
        self.actions = ['UP', 'LEFT', 'RIGHT', 'STOP']

    def load_data(self):
        # Load and normalize dataset
        data = np.load('dataset.npy')  # Assume dataset is stored as a numpy array
        X, y = data[:, :-1], data[:, -1].astype(int)
        X = self.scaler.fit_transform(X)
        return X, y

    def add_noise(self, X, noise_level=0.01):
        noise = np.random.normal(0, noise_level, X.shape)
        return X + noise

    def train(self, epochs=100, batch_size=32):
        X_train, y_train = self.load_data()
        X_train = self.add_noise(X_train)

        X_tensor = torch.tensor(X_train, dtype=torch.float32)
        y_tensor = torch.tensor(y_train, dtype=torch.long)

        dataset = torch.utils.data.TensorDataset(X_tensor, y_tensor)
        dataloader = torch.utils.data.DataLoader(dataset, batch_size=batch_size, shuffle=True)

        criterion = nn.CrossEntropyLoss()
        optimizer = optim.Adam(self.model.parameters(), lr=0.01)
        scheduler = optim.lr_scheduler.StepLR(optimizer, step_size=20, gamma=0.5)

        for epoch in range(epochs):
            for X_batch, y_batch in dataloader:
                optimizer.zero_grad()
                outputs = self.model(X_batch)
                loss = criterion(outputs, y_batch)
                loss.backward()
                optimizer.step()
            scheduler.step()

            if (epoch + 1) % 10 == 0:
                print(f'Epoch [{epoch+1}/{epochs}], Loss: {loss.item():.4f}')

    def act(self, robot_pos, goal_pos, dist_sensors, epsilon=0.1):
        input_data = np.array([robot_pos + goal_pos + dist_sensors])
        input_data = self.scaler.transform(input_data)
        input_tensor = torch.tensor(input_data, dtype=torch.float32)
        
        if random.random() < epsilon:
            return random.choice(self.actions)
        
        with torch.no_grad():
            output = self.model(input_tensor)
            action_index = torch.argmax(output).item()
            return self.actions[action_index]


# class MLAgent(Agent):
#     def __init__(self):
#         model_path = './models/agent_mdl.pkl'
#         load_success = self.load(model_path)
#         if not load_success:
#             # TODO: generate your own data
#             df = pd.read_csv(f'./data/robot_recording_sample.csv')

#             # Here is an example of feature engineering
#             # This creates features to encode the robots rotation in radians and as well as in an xy heading format
#             # these may or may not be useful
#             # TODO: try engineering different features, consider what would be useful and most general
#             df['robot_dir_rads'] = np.radians(df['robot_pos_theta'])
#             df['robot_dir_x'] = np.cos(df['robot_dir_rads'])
#             df['robot_dir_y'] = np.sin(df['robot_dir_rads'])

#             print(df.columns)

#             # TODO: try removing features or adding your own engineered features
#             default_features = ['robot_pos_x', 'robot_pos_y', 'robot_pos_theta', 'goal_pos_x',
#                 'goal_pos_y', 'dist_sensor_0', 'dist_sensor_1', 'dist_sensor_2',
#                 'dist_sensor_3', 'dist_sensor_4', 'dist_sensor_5', 'dist_sensor_6',
#                 'dist_sensor_7', 'dist_sensor_8', 'dist_sensor_9', 'dist_sensor_10',
#                 'dist_sensor_11', 'dist_sensor_12', 'dist_sensor_13', 'dist_sensor_14',
#                 'dist_sensor_15']

#             restricted_features = ['dist_sensor_0', 'dist_sensor_1', 'dist_sensor_2',
#                 'dist_sensor_3', 'dist_sensor_4', 'dist_sensor_5', 'dist_sensor_6',
#                 'dist_sensor_7', 'dist_sensor_8', 'dist_sensor_9', 'dist_sensor_10',
#                 'dist_sensor_11', 'dist_sensor_12', 'dist_sensor_13', 'dist_sensor_14',
#                 'dist_sensor_15', 'robot_dir_x', 'robot_dir_y']
            
#             features = default_features

#             # This will define what you model is trying to predict
#             # In this case it will be the user commands, "UP", "LEFT", "RIGHT", "STOP"
#             labels = ['command_type']

#             # For convenience convert the data to numpy arrays
#             X = df[features].to_numpy()
#             Y = df[labels].to_numpy().flatten()

#             # TODO: try different ml models with different parameters see the README for more info
#             self.model = KNeighborsClassifier(n_neighbors=2, weights='distance')

#             self.train(X, Y)

#             self.save(model_path)

#     def train(self, x_train, y_train):
#         # If you are just using sklearn don't worry about this
#         print("Training...", end='')
#         self.model.fit(x_train, y_train)
#         print("Done")

#     def act(self, robot_pos, goal_pos, dist_sensors):
#         # TODO: make sure you compute the same engineered feaures your ml model expects
#         robot_theta = np.radians(robot_pos[2])
#         robot_dir_x = np.cos(robot_theta)
#         robot_dir_y = np.sin(robot_theta)

#         # TODO: make sure your feature vector matches what your model was trained with
#         default_feature_vec = [*robot_pos, *goal_pos, *dist_sensors]
#         restricted_feature_vec = [*dist_sensors, robot_dir_x, robot_dir_y]
        
#         result = self.model.predict([np.array(default_feature_vec)])
        
#         # You don't need to change anything beyond this point 
#         # if you are predicting the user commands:  "UP", "LEFT", "RIGHT", "STOP"
#         robot_rot_radians = np.radians(robot_pos[2])
#         trajectory = np.array([
#             np.cos(robot_rot_radians),
#             np.sin(robot_rot_radians)
#         ])

#         if result[0] == "LEFT":
#             trajectory = rotation_matrix(np.radians(90)) @ trajectory
#         elif result[0] == "RIGHT":
#             trajectory = rotation_matrix(np.radians(-90)) @ trajectory
#         elif result[0] == "STOP":
#             trajectory = np.zeros(2)

#         return trajectory

#     def save(self, out_path):
#         pkl.dump(self.model, open(out_path, 'wb')) 

#     def load(self, file_path):
#         if os.path.isfile(file_path):
#             print("Model loaded successfully")
#             self.model = pkl.load(open(file_path, 'rb'))
#             return True
#         return False