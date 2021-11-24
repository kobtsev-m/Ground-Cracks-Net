import torch
import pandas as pd

# Data parse
images_df = pd.read_csv('../../dataset/images_train.csv', header=None)
labels_df = pd.read_csv('../../dataset/labels_train.csv', header=None)

images = torch.tensor(images_df.to_numpy(), dtype=torch.float32)
labels = torch.tensor(labels_df.to_numpy(), dtype=torch.float32)

# Model init
input_dim = images.shape[1]
output_dim = labels.shape[1]
model = torch.nn.Linear(input_dim, output_dim)

# Loss & Optimizer
learning_rate = 0.1
criterion = torch.nn.MSELoss()
optimizer = torch.optim.SGD(model.parameters(), lr=learning_rate)

# Training
n_epochs = 150000
for epoch in range(n_epochs):
    target = model(images)
    loss = criterion(labels, target)
    loss.backward()
    optimizer.step()
    optimizer.zero_grad()

    if (epoch + 1) % 10000 == 0:
        print(f'epoch: {epoch + 1} | loss: {loss:.3f}')

torch.save(model, '../artifacts/model.pt')
