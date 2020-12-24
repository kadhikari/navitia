""" Adding attributes additional_time_after_first_section_taxi and
    additional_time_before_last_section_taxi in table instance
Revision ID: 52d628e535db
Revises: 584590c53431
Create Date: 2019-03-19 14:51:41.599764

"""

# revision identifiers, used by Alembic.
revision = '52d628e535db'
down_revision = '584590c53431'

from alembic import op
import sqlalchemy as sa


def upgrade():
    op.add_column(
        'instance',
        sa.Column(
            'additional_time_after_first_section_taxi', sa.Integer(), nullable=False, server_default='300'
        ),
    )
    op.add_column(
        'instance',
        sa.Column(
            'additional_time_before_last_section_taxi', sa.Integer(), nullable=False, server_default='300'
        ),
    )


def downgrade():
    op.drop_column('instance', 'additional_time_before_last_section_taxi')
    op.drop_column('instance', 'additional_time_after_first_section_taxi')
